/**
 * ==============================================================================
 * @file    main_spi_dma_pingpong.c
 * @brief   LESSON 3: High-Speed Continuous Streaming via Circular DMA (Ping-Pong)
 * @author  Divyani Patel
 * ==============================================================================
 * 
 * CORE CONCEPT:
 * - When data rate is very high (e.g. 10 MHz SPI ADC/Audio), interrupting on
 *   every single byte overwhelms the CPU with context switching overhead.
 * - SOLUTION (Zero-Copy Ping-Pong DMA):
 *   1. Allocate a double-buffer of size 2N (e.g., 256 bytes total).
 *   2. DMA Channel 2 silently moves bytes from SPI1_DR into SRAM in hardware.
 *   3. When the 1st half (Ping, bytes 0..127) fills, DMA fires Half-Transfer (HT) ISR.
 *      -> CPU processes Ping while DMA silently continues filling Pong (128..255).
 *   4. When the 2nd half (Pong, bytes 128..255) fills, DMA fires Transfer-Complete (TC) ISR.
 *      -> CPU processes Pong while DMA wraps around and fills Ping (0..127).
 *   5. Zero CPU cycles wasted during transmission!
 * ==============================================================================
 */

#include <stdint.h>
#include <stdbool.h>
#include "spi.h"

/* Double Buffer size: 256 bytes total (128 bytes Ping + 128 bytes Pong) */
#define DMA_TOTAL_BUFFER_SIZE   (256U)
#define DMA_HALF_BUFFER_SIZE    (DMA_TOTAL_BUFFER_SIZE / 2U)

/* Continuous DMA memory storage */
static uint8_t g_dma_rx_buffer[DMA_TOTAL_BUFFER_SIZE];

/* Counters to track processed blocks */
static volatile uint32_t g_ping_blocks_processed = 0U;
static volatile uint32_t g_pong_blocks_processed = 0U;

/**
 * @brief Callback 1: Half-Transfer Complete (Ping)
 * Called from DMA1_Channel2_IRQHandler when bytes 0 to 127 are ready.
 */
static void on_dma_ping_half_ready(uint8_t *data_half, uint16_t length)
{
    g_ping_blocks_processed++;

    /*
     * CPU can safely read/process data_half[0..127] right now!
     * DMA is currently writing to the SECOND half (g_dma_rx_buffer[128..255])
     * so there is ZERO memory collision or race condition!
     */
    (void)data_half;
    (void)length;
}

/**
 * @brief Callback 2: Full-Transfer Complete (Pong)
 * Called from DMA1_Channel2_IRQHandler when bytes 128 to 255 are ready.
 */
static void on_dma_pong_half_ready(uint8_t *data_half, uint16_t length)
{
    g_pong_blocks_processed++;

    /*
     * CPU can safely read/process data_half[128..255] right now!
     * DMA has wrapped around and is currently writing to the FIRST half (0..127)!
     */
    (void)data_half;
    (void)length;
}

int main(void)
{
    /* 1. Initialize SPI1 Master hardware */
    spi1_init();

    /* 
     * 2. Start Circular DMA Continuous Reception:
     * - Configures DMA1 Channel 2 in CIRCULAR mode.
     * - Enables Half-Transfer (HT) and Transfer-Complete (TC) interrupts in NVIC.
     * - Routes incoming SPI1 bytes directly into g_dma_rx_buffer without CPU!
     */
    spi1_dma_rx_circular_init(g_dma_rx_buffer, 
                              DMA_TOTAL_BUFFER_SIZE,
                              on_dma_ping_half_ready, 
                              on_dma_pong_half_ready);

    /* 3. Main Loop */
    while (1)
    {
        /*
         * CPU is 100% free while DMA handles continuous data streaming!
         * Sleep in WFI. Wakes up only when an HT or TC block is ready.
         */
        __asm__ volatile ("wfi");
    }

    return 0;
}
