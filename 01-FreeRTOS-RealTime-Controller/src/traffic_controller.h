/**
 * ==============================================================================
 * @file    traffic_controller.h
 * @brief   Intelligent Real-Time Traffic Signal Controller using FreeRTOS
 * @author  Divyani Patel (IEEE Publication)
 * ==============================================================================
 */

#ifndef TRAFFIC_CONTROLLER_H
#define TRAFFIC_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

#define NUM_LANES                   (4U)
#define SENSOR_QUEUE_LENGTH         (16U)
#define MIN_GREEN_TIME_SEC          (10U)
#define MAX_GREEN_TIME_SEC          (60U)

/* Traffic Light States */
typedef enum {
    SIGNAL_RED = 0,
    SIGNAL_YELLOW,
    SIGNAL_GREEN
} signal_color_t;

/* Sensor Ingestion Event Packet */
typedef struct {
    uint8_t  lane_id;           /* Lane 0 to 3 */
    uint32_t timestamp_ms;      /* Hardware timer timestamp */
    uint16_t vehicle_pulse_cnt; /* Detected vehicle count */
} sensor_event_t;

/* Global Intersection State Protected by Mutex */
typedef struct {
    signal_color_t lane_states[NUM_LANES];
    uint32_t       current_green_lane;
    uint32_t       density_weights[NUM_LANES];
    bool           emergency_override_active;
} intersection_state_t;

/* Task Prototypes */
void vSensorIngestionTask(void *pvParameters);
void vTimingAlgorithmTask(void *pvParameters);
void vSignalActuationTask(void *pvParameters);

/* ISR Notification Prototype */
void Sensor_Pulse_IRQHandler(uint8_t lane_id);

#endif /* TRAFFIC_CONTROLLER_H */
