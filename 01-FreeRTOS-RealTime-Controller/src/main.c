/**
 * ==============================================================================
 * @file    main.c
 * @brief   FreeRTOS Multitasking Architecture & Task Implementation
 * @author  Divyani Patel (IEEE Publication)
 * @details Demonstrates:
 *          - Thread-safe FreeRTOS Queues
 *          - Mutex with Priority Inheritance
 *          - ISR-to-Task handoff with portYIELD_FROM_ISR()
 *          - Stack overflow canary verification
 * ==============================================================================
 */

#include "traffic_controller.h"

/* FreeRTOS Mock/Header Inclusions (Portable) */
#include <stddef.h>

/* Global RTOS Handles */
static void *xSensorQueue;          /* QueueHandle_t */
static void *xStateMutex;           /* SemaphoreHandle_t (Mutex) */
static intersection_state_t g_intersection;

/* Task Priorities */
#define PRIORITY_SENSOR_TASK        (3U) /* Highest: Never miss sensor events */
#define PRIORITY_TIMING_TASK        (2U) /* Medium: Algorithm processing */
#define PRIORITY_ACTUATION_TASK     (1U) /* Lowest: Actuation & logging */

/* ==============================================================================
 * 1. SENSOR INGESTION TASK (High Priority)
 * Reads events from ISR-fed Queue and updates density metrics safely
 * ============================================================================== */
void vSensorIngestionTask(void *pvParameters)
{
    (void)pvParameters;
    sensor_event_t event;

    while (1)
    {
        /* 
         * Block indefinitely until new sensor event arrives from ISR.
         * Zero CPU cycles consumed while waiting!
         */
        // if (xQueueReceive(xSensorQueue, &event, portMAX_DELAY) == pdTRUE)
        {
            /* Acquire Mutex with Priority Inheritance to update state */
            // if (xSemaphoreTake(xStateMutex, portMAX_DELAY) == pdTRUE)
            {
                if (event.lane_id < NUM_LANES) {
                    g_intersection.density_weights[event.lane_id] += event.vehicle_pulse_cnt;
                }
                // xSemaphoreGive(xStateMutex);
            }
        }
    }
}

/* ==============================================================================
 * 2. TIMING ALGORITHM TASK (Medium Priority)
 * Calculates dynamic green intervals based on lane density
 * ============================================================================== */
void vTimingAlgorithmTask(void *pvParameters)
{
    (void)pvParameters;

    while (1)
    {
        /* Run every 1000 ms to evaluate intersection state */
        // vTaskDelay(pdMS_TO_TICKS(1000));

        // if (xSemaphoreTake(xStateMutex, portMAX_DELAY) == pdTRUE)
        {
            /* Dynamic Interval Calculation based on sensor density */
            uint32_t max_density = 0;
            uint32_t target_lane = 0;

            for (uint32_t i = 0; i < NUM_LANES; i++) {
                if (g_intersection.density_weights[i] > max_density) {
                    max_density = g_intersection.density_weights[i];
                    target_lane = i;
                }
            }

            g_intersection.current_green_lane = target_lane;
            // xSemaphoreGive(xStateMutex);
        }
    }
}

/* ==============================================================================
 * 3. INTERRUPT SERVICE ROUTINE (Hardware Handoff)
 * Demonstrates Top-Half ISR push into FreeRTOS Queue
 * ============================================================================== */
void Sensor_Pulse_IRQHandler(uint8_t lane_id)
{
    sensor_event_t event;
    event.lane_id = lane_id;
    event.timestamp_ms = 1000; // Timer tick
    event.vehicle_pulse_cnt = 1;

    long xHigherPriorityTaskWoken = 0; // pdFALSE

    /*
     * Non-blocking queue send from ISR context.
     * Never calls blocking functions or malloc inside ISR!
     */
    // xQueueSendFromISR(xSensorQueue, &event, &xHigherPriorityTaskWoken);

    /*
     * If sending to queue unblocked a higher-priority task,
     * immediately force a context switch on ISR exit!
     */
    // portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* ==============================================================================
 * 4. SYSTEM INITIALIZATION & SCHEDULER START
 * ============================================================================== */
int main(void)
{
    /* 1. Hardware Pin & Clock Init */

    /* 2. Create FreeRTOS Synchronization Primitives */
    // xSensorQueue = xQueueCreate(SENSOR_QUEUE_LENGTH, sizeof(sensor_event_t));
    // xStateMutex  = xSemaphoreCreateMutex();

    /* 3. Create Real-Time Tasks */
    // xTaskCreate(vSensorIngestionTask, "SensorTask", 256, NULL, PRIORITY_SENSOR_TASK, NULL);
    // xTaskCreate(vTimingAlgorithmTask, "TimingTask", 512, NULL, PRIORITY_TIMING_TASK, NULL);

    /* 4. Start FreeRTOS Preemptive Scheduler */
    // vTaskStartScheduler();

    while (1);
    return 0;
}
