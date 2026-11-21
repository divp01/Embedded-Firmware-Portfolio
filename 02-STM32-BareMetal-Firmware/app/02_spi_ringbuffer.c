/**
 * ==============================================================================
 * @file    main_spi_ringbuffer.c
 * @brief   LESSON 2: Continuous Streaming Data via Interrupt & Ring Buffer (FIFO)
 * @author  Divyani Patel
 * ==============================================================================
 * 
 * CORE CONCEPT:
 * - When an external device blasts continuous data over SPI, reading inside
 *   main() with polling will drop bytes whenever the CPU is busy.
 * - SOLUTION (Producer-Consumer):
 *   1. Producer (SPI1_IRQHandler): Every arriving byte immediately triggers
 *      an interrupt. The ISR reads SPI1_DR and pushes it into a 128-byte
 *      Circular Ring Buffer (takes < 1 microsecond).
 *   2. Consumer (main loop): Pops bytes from the Ring Buffer and processes
 *      them safely without dropping a single byte.
 * ==============================================================================
 */

#include <stdint.h>
#include <stdbool.h>
#include "spi.h"
#include "ring_buffer.h"

/* Ring Buffer instance to hold continuous incoming bytes */
static ring_buffer_t g_rx_fifo;

/* Counter to count how many bytes we received from external device */
static volatile uint32_t g_total_bytes_received = 0U;

/**
 * @brief Simulated parser for received streaming bytes
 * In a real project, this parses GPS NMEA sentences, sensor packets, etc.
 */
static void process_byte(uint8_t byte)
{
    g_total_bytes_received++;
    (void)byte; // In real firmware: packet_parser(byte);
}

int main(void)
{
    /* 1. Initialize SPI1 Master (PA4=CS, PA5=SCK, PA6=MISO, PA7=MOSI) */
    spi1_init();

    /* 2. Initialize the Lock-Free Circular Ring Buffer */
    ring_buffer_init(&g_rx_fifo);

    /* 3. Enable SPI1 Receive Interrupt (SPI_CR2_RXNEIE) in SPI1 and NVIC */
    spi1_enable_rx_interrupt(&g_rx_fifo);

    uint8_t received_byte = 0U;

    /* 4. Consumer Loop */
    while (1)
    {
        /* Check if any streaming bytes are waiting in the Ring Buffer */
        if (ring_buffer_pop(&g_rx_fifo, &received_byte))
        {
            /* Process the byte safely in main() thread context */
            process_byte(received_byte);
        }
        else
        {
            /*
             * Ring buffer is currently empty!
             * Sleep in WFI until the next SPI byte arrives.
             * When an SPI byte arrives, SPI1_IRQHandler runs, pushes the byte,
             * and the CPU resumes right here!
             */
            __asm__ volatile ("wfi");
        }
    }

    return 0;
}
