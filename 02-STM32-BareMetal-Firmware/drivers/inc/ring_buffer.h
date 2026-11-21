/**
 * ==============================================================================
 * @file    ring_buffer.h
 * @brief   High-Speed Lock-Free Circular Ring Buffer for Interrupts & Streaming
 * @author  Divyani Patel
 * @details Designed specifically for continuous streaming data (SPI, UART, ADC).
 *          - Fast bitwise wrapping using (RING_BUFFER_SIZE - 1)
 *          - Single Producer (ISR) / Single Consumer (main) thread-safety
 *          - Built-in Overrun / Overflow detection
 * ==============================================================================
 */

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

/* Buffer size MUST be a power of 2 (64, 128, 256, 512, 1024) */
#define RING_BUFFER_SIZE    (128U)
#define RING_BUFFER_MASK    (RING_BUFFER_SIZE - 1U)

typedef struct {
    uint8_t buffer[RING_BUFFER_SIZE];
    volatile uint16_t head;       /* Read pointer (consumed by main thread) */
    volatile uint16_t tail;       /* Write pointer (produced by ISR) */
    volatile uint32_t overruns;   /* Dropped byte counter */
} ring_buffer_t;

/**
 * @brief Initializes the ring buffer pointers and overrun counter.
 */
void ring_buffer_init(ring_buffer_t *rb);

/**
 * @brief  Pushes a byte into the ring buffer (called inside ISR).
 * @param  rb: Pointer to ring buffer.
 * @param  data: Byte received from SPI/UART.
 * @return true if stored successfully, false if buffer was FULL (overrun).
 */
bool ring_buffer_push(ring_buffer_t *rb, uint8_t data);

/**
 * @brief  Pops a byte from the ring buffer (called in main thread).
 * @param  rb: Pointer to ring buffer.
 * @param  data: Pointer to store the popped byte.
 * @return true if byte was retrieved, false if buffer is EMPTY.
 */
bool ring_buffer_pop(ring_buffer_t *rb, uint8_t *data);

/**
 * @brief  Checks if the ring buffer has no data.
 */
bool ring_buffer_is_empty(const ring_buffer_t *rb);

/**
 * @brief  Returns the number of unread bytes available in the buffer.
 */
uint16_t ring_buffer_available(const ring_buffer_t *rb);

#endif /* RING_BUFFER_H */
