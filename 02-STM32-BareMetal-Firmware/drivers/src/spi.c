/**
 * ==============================================================================
 * @file    spi.c
 * @brief   Bare-Metal SPI1 Driver Implementation with Continuous Streaming
 * @author  Divyani Patel
 * ==============================================================================
 */

#include "spi.h"
#include <stddef.h>

/* ==============================================================================
 * HARDWARE REGISTERS (RCC, GPIOA, NVIC)
 * ============================================================================== */
#define RCC_BASE            (0x40021000UL)
#define RCC_AHBENR          (*(volatile uint32_t *)(RCC_BASE + 0x14))
#define RCC_APB2ENR         (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_AHBENR_DMA1EN   (1U << 0)   /* DMA1 Clock Enable */
#define RCC_APB2ENR_IOPAEN  (1U << 2)   /* Port A Clock Enable */
#define RCC_APB2ENR_SPI1EN  (1U << 12)  /* SPI1 Clock Enable */

#define GPIOA_BASE          (0x40010800UL)
#define GPIOA_CRL           (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR           (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))
#define GPIOA_BSRR          (*(volatile uint32_t *)(GPIOA_BASE + 0x10))

#define CS_PIN              (4U)        /* PA4 */

/* ARM Cortex-M3 NVIC Registers */
#define NVIC_ISER0          (*(volatile uint32_t *)(0xE000E100UL)) /* IRQs 0-31 */
#define NVIC_ISER1          (*(volatile uint32_t *)(0xE000E104UL)) /* IRQs 32-63 */

/* ==============================================================================
 * PRIVATE STATE
 * ============================================================================== */
static ring_buffer_t   *s_spi_rx_ring_buffer = NULL;
static dma_callback_t   s_dma_ht_callback    = NULL;
static dma_callback_t   s_dma_tc_callback    = NULL;
static uint8_t         *s_dma_buffer         = NULL;
static uint16_t         s_dma_half_size      = 0U;

/* ==============================================================================
 * 1. BASIC INITIALIZATION & POLLING
 * ============================================================================== */
void spi1_init(void)
{
    /* Enable Clocks for GPIOA and SPI1 */
    RCC_APB2ENR |= (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_SPI1EN);

    /* Configure GPIO Pins: PA4=Out(0x1), PA5=AF(0x9), PA6=In(0x8), PA7=AF(0x9) */
    GPIOA_CRL &= ~(0xFFFF0000UL);
    GPIOA_CRL |=  (0x98910000UL);
    GPIOA_ODR |=  (1U << 6); // Pull-up on MISO

    /* Deselect CS (PA4 HIGH) */
    spi1_cs_disable();

    /* Configure SPI1: Master, fPCLK/8 (1 MHz), CPOL=0, CPHA=0, SSM=1, SSI=1 */
    SPI1_CR1 = SPI_CR1_SSM     |
               SPI_CR1_SSI     |
               SPI_CR1_MSTR    |
               SPI_CR1_BR_DIV8 |
               SPI_CR1_SPE;
}

void spi1_cs_enable(void)
{
    GPIOA_BSRR = (1U << (CS_PIN + 16U)); /* PA4 LOW */
}

void spi1_cs_disable(void)
{
    GPIOA_BSRR = (1U << CS_PIN);         /* PA4 HIGH */
}

uint8_t spi1_transfer(uint8_t tx_data)
{
    while (!(SPI1_SR & SPI_SR_TXE));
    SPI1_DR = tx_data;
    while (!(SPI1_SR & SPI_SR_RXNE));
    return (uint8_t)(SPI1_DR & 0xFF);
}

void spi1_write(uint8_t data)
{
    (void)spi1_transfer(data);
}

uint8_t spi1_read(void)
{
    return spi1_transfer(0xFF);
}

/* ==============================================================================
 * 2. INTERRUPT-DRIVEN CONTINUOUS STREAMING (RING BUFFER)
 * ============================================================================== */
void spi1_enable_rx_interrupt(ring_buffer_t *rb)
{
    s_spi_rx_ring_buffer = rb;

    /* Enable RXNE interrupt in SPI1 Control Register 2 */
    SPI1_CR2 |= SPI_CR2_RXNEIE;

    /* Enable SPI1 IRQ (IRQ #35 -> bit 3 in NVIC_ISER1) */
    NVIC_ISER1 = (1U << (35U - 32U));
}

/**
 * @brief SPI1 Interrupt Service Routine
 * Fired whenever a new byte arrives from an external device on MISO.
 */
void SPI1_IRQHandler(void)
{
    /* Check if RXNE flag is set */
    if (SPI1_SR & SPI_SR_RXNE)
    {
        uint8_t byte = (uint8_t)(SPI1_DR & 0xFF); /* Reading DR automatically clears RXNE */

        if (s_spi_rx_ring_buffer != NULL) {
            ring_buffer_push(s_spi_rx_ring_buffer, byte);
        }
    }

    /* Check and clear Overrun Error if main thread was too slow */
    if (SPI1_SR & SPI_SR_OVR) {
        volatile uint32_t dummy = SPI1_DR;
        dummy = SPI1_SR;
        (void)dummy;
    }
}

/* ==============================================================================
 * 3. ZERO-COPY CIRCULAR DMA STREAMING (PING-PONG)
 * ============================================================================== */
void spi1_dma_rx_circular_init(uint8_t *double_buffer, uint16_t total_size,
                               dma_callback_t half_complete_cb,
                               dma_callback_t full_complete_cb)
{
    s_dma_buffer        = double_buffer;
    s_dma_half_size     = total_size / 2U;
    s_dma_ht_callback   = half_complete_cb;
    s_dma_tc_callback   = full_complete_cb;

    /* 1. Enable DMA1 Clock */
    RCC_AHBENR |= RCC_AHBENR_DMA1EN;

    /* 2. Disable Channel 2 before configuring */
    DMA1_CCR2 = 0U;

    /* 3. Set Peripheral and Memory Addresses */
    DMA1_CPAR2 = (uint32_t)(&SPI1_DR);
    DMA1_CMAR2 = (uint32_t)double_buffer;
    DMA1_CNDTR2 = total_size;

    /* 4. Configure DMA Channel 2:
     *    - Memory increment enabled (MINC)
     *    - Circular mode enabled (CIRC)
     *    - Half-Transfer Interrupt Enabled (HTIE)
     *    - Transfer-Complete Interrupt Enabled (TCIE)
     */
    DMA1_CCR2 = DMA_CCR_MINC |
                DMA_CCR_CIRC |
                DMA_CCR_HTIE |
                DMA_CCR_TCIE;

    /* 5. Enable DMA1 Channel 2 IRQ in NVIC (IRQ 12) */
    NVIC_ISER0 = (1U << 12U);

    /* 6. Enable SPI1 RX DMA in SPI1_CR2 */
    SPI1_CR2 |= SPI_CR2_RXDMAEN;

    /* 7. Start DMA Channel */
    DMA1_CCR2 |= DMA_CCR_EN;
}

/**
 * @brief DMA1 Channel 2 ISR (SPI1_RX DMA)
 * Handles Half-Transfer (Ping) and Transfer-Complete (Pong) events.
 */
void DMA1_Channel2_IRQHandler(void)
{
    /* 1. HALF-TRANSFER EVENT (First half of buffer is full: 0 to N-1) */
    if (DMA1_ISR & DMA_ISR_HTIF2)
    {
        /* Clear the Half-Transfer flag */
        DMA1_IFCR = DMA_IFCR_CHTIF2;

        /* Notify application: CPU can safely process the 1st half
         * while DMA continues filling the 2nd half in the background! */
        if (s_dma_ht_callback != NULL) {
            s_dma_ht_callback(&s_dma_buffer[0], s_dma_half_size);
        }
    }

    /* 2. TRANSFER-COMPLETE EVENT (Second half of buffer is full: N to 2N-1) */
    if (DMA1_ISR & DMA_ISR_TCIF2)
    {
        /* Clear the Transfer-Complete flag */
        DMA1_IFCR = DMA_IFCR_CTCIF2;

        /* Notify application: CPU can safely process the 2nd half
         * while DMA wraps around to index 0 and fills the 1st half! */
        if (s_dma_tc_callback != NULL) {
            s_dma_tc_callback(&s_dma_buffer[s_dma_half_size], s_dma_half_size);
        }
    }
}
