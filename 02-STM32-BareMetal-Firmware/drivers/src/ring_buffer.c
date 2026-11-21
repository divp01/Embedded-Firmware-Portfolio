/**
 * ==============================================================================
 * @file    ring_buffer.c
 * @brief   Lock-Free Circular Ring Buffer Implementation
 * @author  Divyani Patel
 * ==============================================================================
 */

#include "ring_buffer.h"

void ring_buffer_init(ring_buffer_t *rb)
{
    rb->head = 0U;
    rb->tail = 0U;
    rb->overruns = 0U;
}

bool ring_buffer_push(ring_buffer_t *rb, uint8_t data)
{
    /* Calculate next tail index using bitwise mask instead of slow division */
    uint16_t next_tail = (rb->tail + 1U) & RING_BUFFER_MASK;

    /* If next_tail equals head, the buffer is FULL! Cannot push. */
    if (next_tail == rb->head) {
        rb->overruns++; // Record dropped byte
        return false;
    }

    /* Store byte and advance tail pointer */
    rb->buffer[rb->tail] = data;
    rb->tail = next_tail;
    return true;
}

bool ring_buffer_pop(ring_buffer_t *rb, uint8_t *data)
{
    /* If head equals tail, buffer is EMPTY */
    if (rb->head == rb->tail) {
        return false;
    }

    /* Read byte and advance head pointer */
    *data = rb->buffer[rb->head];
    rb->head = (rb->head + 1U) & RING_BUFFER_MASK;
    return true;
}

bool ring_buffer_is_empty(const ring_buffer_t *rb)
{
    return (rb->head == rb->tail);
}

uint16_t ring_buffer_available(const ring_buffer_t *rb)
{
    return (rb->tail - rb->head) & RING_BUFFER_MASK;
}
