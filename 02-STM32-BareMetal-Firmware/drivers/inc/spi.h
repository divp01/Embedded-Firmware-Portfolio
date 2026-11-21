/**
 * ==============================================================================
 * @file    spi.h
 * @brief   Bare-Metal SPI1 Driver with Continuous Streaming Support
 * @author  Divyani Patel
 * @details Supports 3 Modes:
 *          1. Polling Master Mode
 *          2. Interrupt-Driven Continuous Streaming (Ring Buffer)
 *          3. Zero-Copy Circular DMA with Half-Transfer (HT) & Transfer-Complete (TC)
 *
 * Pin Mapping on Blue Pill (Port A):
 *   - PA4: CS / NSS  (Software-controlled GPIO Output)
 *   - PA5: SCK       (Alternate Function Push-Pull)
 *   - PA6: MISO      (Input Floating / Pull-Up)
 *   - PA7: MOSI      (Alternate Function Push-Pull)
 * ==============================================================================
 */

#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <stdbool.h>
#include "ring_buffer.h"

/* ==============================================================================
 * HARDWARE REGISTER ADDRESSES (STM32F103x8 RM0008)
 * ============================================================================== */

/* SPI1 Peripheral Base Address on APB2 Bus */
#define SPI1_BASE           (0x40013000UL)
#define SPI1_CR1            (*(volatile uint32_t *)(SPI1_BASE + 0x00))
#define SPI1_CR2            (*(volatile uint32_t *)(SPI1_BASE + 0x04))
#define SPI1_SR             (*(volatile uint32_t *)(SPI1_BASE + 0x08))
#define SPI1_DR             (*(volatile uint32_t *)(SPI1_BASE + 0x0C))

/* SPI1 Status Register (SR) Flags */
#define SPI_SR_RXNE         (1U << 0)  /* Receive buffer not empty */
#define SPI_SR_TXE          (1U << 1)  /* Transmit buffer empty */
#define SPI_SR_OVR          (1U << 6)  /* Overrun flag */
#define SPI_SR_BSY          (1U << 7)  /* Busy flag */

/* SPI1 Control Register 1 (CR1) Bits */
#define SPI_CR1_CPHA        (1U << 0)
#define SPI_CR1_CPOL        (1U << 1)
#define SPI_CR1_MSTR        (1U << 2)
#define SPI_CR1_BR_DIV8     (2U << 3)  /* 8 MHz / 8 = 1 MHz */
#define SPI_CR1_SPE         (1U << 6)
#define SPI_CR1_SSI         (1U << 8)
#define SPI_CR1_SSM         (1U << 9)

/* SPI1 Control Register 2 (CR2) Bits */
#define SPI_CR2_RXDMAEN     (1U << 0)  /* Rx Buffer DMA Enable */
#define SPI_CR2_TXDMAEN     (1U << 1)  /* Tx Buffer DMA Enable */
#define SPI_CR2_RXNEIE      (1U << 6)  /* RX Buffer Not Empty Interrupt Enable */

/* ==============================================================================
 * DMA1 REGISTERS (Channel 2 is dedicated to SPI1_RX on STM32F103)
 * ============================================================================== */
#define DMA1_BASE           (0x40020000UL)
#define DMA1_ISR            (*(volatile uint32_t *)(DMA1_BASE + 0x00)) /* Status Register */
#define DMA1_IFCR           (*(volatile uint32_t *)(DMA1_BASE + 0x04)) /* Flag Clear Register */

/* DMA1 Channel 2 (SPI1_RX) Registers */
#define DMA1_CCR2           (*(volatile uint32_t *)(DMA1_BASE + 0x1C))
#define DMA1_CNDTR2         (*(volatile uint32_t *)(DMA1_BASE + 0x20))
#define DMA1_CPAR2          (*(volatile uint32_t *)(DMA1_BASE + 0x24))
#define DMA1_CMAR2          (*(volatile uint32_t *)(DMA1_BASE + 0x28))

/* DMA Flags for Channel 2 */
#define DMA_ISR_TCIF2       (1U << 5)  /* Transfer Complete Flag */
#define DMA_ISR_HTIF2       (1U << 6)  /* Half-Transfer Flag */
#define DMA_IFCR_CTCIF2     (1U << 5)  /* Clear Transfer Complete */
#define DMA_IFCR_CHTIF2     (1U << 6)  /* Clear Half-Transfer */
#define DMA_IFCR_CGIF2      (1U << 4)  /* Clear Global Interrupt */

/* DMA CCR Bits */
#define DMA_CCR_EN          (1U << 0)  /* Channel Enable */
#define DMA_CCR_TCIE        (1U << 1)  /* Transfer Complete Interrupt Enable */
#define DMA_CCR_HTIE        (1U << 2)  /* Half Transfer Interrupt Enable */
#define DMA_CCR_CIRC        (1U << 5)  /* Circular Mode */
#define DMA_CCR_MINC        (1U << 7)  /* Memory Increment Mode */

/* ==============================================================================
 * PUBLIC DRIVER APIS
 * ============================================================================== */

/* 1. Basic Master Init & Polling APIs */
void    spi1_init(void);
void    spi1_cs_enable(void);
void    spi1_cs_disable(void);
uint8_t spi1_transfer(uint8_t tx_data);
void    spi1_write(uint8_t data);
uint8_t spi1_read(void);

/* 2. Interrupt-Driven Streaming with Ring Buffer */
void    spi1_enable_rx_interrupt(ring_buffer_t *rb);

/* 3. Circular DMA Continuous Streaming (Zero-Copy Ping-Pong) */
typedef void (*dma_callback_t)(uint8_t *data_half, uint16_t length);

void    spi1_dma_rx_circular_init(uint8_t *double_buffer, uint16_t total_size,
                                  dma_callback_t half_complete_cb,
                                  dma_callback_t full_complete_cb);

#endif /* SPI_H */
