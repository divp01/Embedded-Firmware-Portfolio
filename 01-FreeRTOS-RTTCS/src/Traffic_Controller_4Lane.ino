/**
 * ==============================================================================
 * @file    Traffic_Controller_4Lane.ino
 * @brief   Intelligent Real-Time Traffic Signal Control System Using FreeRTOS
 * @author  Divyani Patel (23MRE005 - Nirma University & IEEE Xplore)
 * @target  ATmega2560 (Arduino Mega 2560 @ 16 MHz)
 * @details Implements 4-Lane dynamic traffic density scheduling using FreeRTOS.
 *          Sensors: 8 IR sensors (High and Low pairs for 4 lanes).
 *          Outputs: 12 Traffic LEDs (R, Y, G for 4 lanes) + 7-Segment Countdown.
 *          Validation: Tested & verified with Digilent Analog Discovery 2 Kit.
 * ==============================================================================
 */

#include <Arduino_FreeRTOS.h>
#include "task.h"

#define configUSE_PREEMPTION    0
#define configUSE_TIME_SLICING   0
#define configMAX_PRIORITIES    ((UBaseType_t)6)

/* Task Handles */
TaskHandle_t MasterTaskHandle;
TaskHandle_t taskLane1Handle;
TaskHandle_t taskLane2Handle;
TaskHandle_t taskLane3Handle;
TaskHandle_t taskLane4Handle;

static uint8_t Delay_Tick_Array[4] = {5, 5, 5, 5};
const int SEGMENT_PINS[] = {A6, A7, A14, A13, A12, A5, A4}; // Segments A, B, C, D, E, F, G
TickType_t xDelay = pdMS_TO_TICKS(1000); // 1-second delay tick

/* ==============================================================================
 * GPIO PIN DEFINITIONS
 * ============================================================================== */

/* IR Proximity Sensors (High & Low density detectors per lane) */
#define Sensor1_1   2  // Lane 1 High Density
#define Sensor1_2   3  // Lane 1 Low Density
#define Sensor2_1   4  // Lane 2 High Density
#define Sensor2_2   5  // Lane 2 Low Density
#define Sensor3_1   6  // Lane 3 High Density
#define Sensor3_2   7  // Lane 3 Low Density
#define Sensor4_1   8  // Lane 4 High Density
#define Sensor4_2   9  // Lane 4 Low Density

/* Traffic Light LEDs */
#define R1 53
#define Y1 51
#define G1 49
#define R2 47
#define Y2 45
#define G2 43
#define R3 41
#define Y3 39
#define G3 37
#define R4 35
#define Y4 33
#define G4 31

/* Function Prototypes */
void MasterTask(void *pvParameters);
void taskLane1(void *pvParameters);
void taskLane2(void *pvParameters);
void taskLane3(void *pvParameters);
void taskLane4(void *pvParameters);
void displayDigit(int digit);

/* ==============================================================================
 * FREERTOS HOOKS
 * ============================================================================== */
void vApplicationTickHook(void) {
    // Custom tick behavior if needed
}

void vApplicationIdleHook(void) {
    // Background idle tasks
}

/* ==============================================================================
 * SYSTEM SETUP
 * ============================================================================== */
void setup() 
{
    Serial.begin(9600);

    // Initialize IR Sensor Inputs
    pinMode(Sensor1_1, INPUT);
    pinMode(Sensor1_2, INPUT);
    pinMode(Sensor2_1, INPUT);
    pinMode(Sensor2_2, INPUT);
    pinMode(Sensor3_1, INPUT);
    pinMode(Sensor3_2, INPUT);
    pinMode(Sensor4_1, INPUT);
    pinMode(Sensor4_2, INPUT);

    // Initialize Traffic Light LED Outputs
    pinMode(R1, OUTPUT); pinMode(Y1, OUTPUT); pinMode(G1, OUTPUT);
    pinMode(R2, OUTPUT); pinMode(Y2, OUTPUT); pinMode(G2, OUTPUT);
    pinMode(R3, OUTPUT); pinMode(Y3, OUTPUT); pinMode(G3, OUTPUT);
    pinMode(R4, OUTPUT); pinMode(Y4, OUTPUT); pinMode(G4, OUTPUT);

    // Initialize 7-Segment Display Pins
    for (int i = 0; i < 7; i++) {
        pinMode(SEGMENT_PINS[i], OUTPUT);
    }

    // Create FreeRTOS Tasks with Initial Priorities
    xTaskCreate(MasterTask, "MasterTask_Sensor", 150, NULL, 4, &MasterTaskHandle);
    xTaskCreate(taskLane1,  "taskLane1",          100, NULL, 3, &taskLane1Handle);
    xTaskCreate(taskLane2,  "taskLane2",          100, NULL, 2, &taskLane2Handle);
    xTaskCreate(taskLane3,  "taskLane3",          100, NULL, 1, &taskLane3Handle);
    xTaskCreate(taskLane4,  "taskLane4",          100, NULL, 0, &taskLane4Handle);
}

void loop() 
{
    // Execution handled entirely by FreeRTOS scheduler
}

/* ==============================================================================
 * 1. MASTER SENSOR & DYNAMIC PRIORITY TASK
 * Samples all 8 IR sensors, calculates traffic density, and dynamically
 * shifts FreeRTOS task priorities using vTaskPrioritySet()!
 * ============================================================================== */
void MasterTask(void *pvParameters) 
{
    (void)pvParameters;
    static volatile bool h1, l1, h2, l2, h3, l3, h4, l4;
    static bool local_Assignment[4];
    static uint8_t Priority_Array[4];

    while (1) 
    {
        Serial.println("Master Started");

        // Read IR Sensors across all 4 lanes
        h1 = digitalRead(Sensor1_1);
        l1 = digitalRead(Sensor1_2);
        h2 = digitalRead(Sensor2_1);
        l2 = digitalRead(Sensor2_2);
        h3 = digitalRead(Sensor3_1);
        l3 = digitalRead(Sensor3_2);
        h4 = digitalRead(Sensor4_1);
        l4 = digitalRead(Sensor4_2);

        // Density allocation: High (both sensors active) vs Normal
        local_Assignment[0] = (h1 & l1);
        local_Assignment[1] = (h2 & l2);
        local_Assignment[2] = (h3 & l3);
        local_Assignment[3] = (h4 & l4);

        // Priority Allocation Logic
        uint8_t c1 = 0, c2 = 3;
        for (int k = 0; k < 4; k++) 
        {
            if (local_Assignment[k] == 1) {
                Priority_Array[k] = c2;
                Delay_Tick_Array[k] = 7; // High density gets 7 ticks green time
                c2--;
            } else {
                Priority_Array[k] = c1;
                Delay_Tick_Array[k] = 5; // Normal density gets 5 ticks green time
                c1++;
            }
        }

        // Dynamically reassign FreeRTOS task priorities based on density!
        vTaskPrioritySet(taskLane1Handle, Priority_Array[0]);
        vTaskPrioritySet(taskLane2Handle, Priority_Array[1]);
        vTaskPrioritySet(taskLane3Handle, Priority_Array[2]);
        vTaskPrioritySet(taskLane4Handle, Priority_Array[3]);

        Serial.print("Lane - Priority : Delay Tick : \n");
        for (int i = 0; i < 4; i++) {
            Serial.print("Lane");
            Serial.print(i + 1);
            Serial.print(" - ");
            Serial.print(Priority_Array[i]);
            Serial.print(": ");
            Serial.print(Delay_Tick_Array[i]);
            Serial.print("\n");
        }

        // Suspend MasterTask until cycle completes
        vTaskSuspend(MasterTaskHandle);
    }
}

/* ==============================================================================
 * 2. LANE 1 TASK
 * ============================================================================== */
void taskLane1(void *pvParameters) 
{
    (void)pvParameters;
    UBaseType_t check;
    TickType_t xLastWakeTime;
    TickType_t xCurrentTime;

    while (1) 
    {
        Serial.println(" Lane-1 is running");

        // Green on Lane 1, Red on all others
        digitalWrite(Y1, LOW);  digitalWrite(Y2, LOW);  digitalWrite(Y3, LOW);  digitalWrite(Y4, LOW);
        digitalWrite(G1, HIGH); digitalWrite(G2, LOW);  digitalWrite(G3, LOW);  digitalWrite(G4, LOW);
        digitalWrite(R1, LOW);  digitalWrite(R2, HIGH); digitalWrite(R3, HIGH); digitalWrite(R4, HIGH);

        // Countdown display for assigned Green Time
        for (int i = Delay_Tick_Array[0]; i >= 1; i--) 
        {
            displayDigit(i);
            xLastWakeTime = xTaskGetTickCount();
            xCurrentTime = xLastWakeTime;
            while ((xCurrentTime - xLastWakeTime) < xDelay) {
                xCurrentTime = xTaskGetTickCount();
            }
        }

        // Yellow transition
        digitalWrite(Y1, HIGH);
        for (int i = 2; i >= 1; i--) 
        {
            displayDigit(i);
            xLastWakeTime = xTaskGetTickCount();
            xCurrentTime = xLastWakeTime;
            while ((xCurrentTime - xLastWakeTime) < xDelay) {
                xCurrentTime = xTaskGetTickCount();
            }
        }

        check = uxTaskPriorityGet(taskLane1Handle);
        // If last priority task in this cycle, resume MasterTask to recalculate priorities!
        if (check == 0) {
            vTaskResume(MasterTaskHandle);
            vTaskResume(taskLane1Handle);
            vTaskResume(taskLane2Handle);
            vTaskResume(taskLane3Handle);
            vTaskResume(taskLane4Handle);
            taskYIELD();
        } else {
            vTaskSuspend(taskLane1Handle);
        }
    }
}

/* ==============================================================================
 * 3. LANE 2 TASK
 * ============================================================================== */
void taskLane2(void *pvParameters) 
{
    (void)pvParameters;
    UBaseType_t check;
    TickType_t xLastWakeTime;
    TickType_t xCurrentTime;

    while (1) 
    {
        Serial.println(" Lane-2 is running");

        // Green on Lane 2, Red on all others
        digitalWrite(Y1, LOW);  digitalWrite(Y2, LOW);  digitalWrite(Y3, LOW);  digitalWrite(Y4, LOW);
        digitalWrite(G1, LOW);  digitalWrite(G2, HIGH); digitalWrite(G3, LOW);  digitalWrite(G4, LOW);
        digitalWrite(R1, HIGH); digitalWrite(R2, LOW);  digitalWrite(R3, HIGH); digitalWrite(R4, HIGH);

        for (int i = Delay_Tick_Array[1]; i >= 1; i--) 
        {
            displayDigit(i);
            xLastWakeTime = xTaskGetTickCount();
            xCurrentTime = xLastWakeTime;
            while ((xCurrentTime - xLastWakeTime) < xDelay) {
                xCurrentTime = xTaskGetTickCount();
            }
        }

        digitalWrite(Y2, HIGH);
        for (int i = 2; i >= 1; i--) 
        {
            displayDigit(i);
            xLastWakeTime = xTaskGetTickCount();
            xCurrentTime = xLastWakeTime;
            while ((xCurrentTime - xLastWakeTime) < xDelay) {
                xCurrentTime = xTaskGetTickCount();
            }
        }

        check = uxTaskPriorityGet(taskLane2Handle);
        if (check == 0) {
            vTaskResume(MasterTaskHandle);
            vTaskResume(taskLane1Handle);
            vTaskResume(taskLane2Handle);
            vTaskResume(taskLane3Handle);
            vTaskResume(taskLane4Handle);
            taskYIELD();
        } else {
            vTaskSuspend(taskLane2Handle);
        }
    }
}

/* ==============================================================================
 * 4. LANE 3 TASK
 * ============================================================================== */
void taskLane3(void *pvParameters) 
{
    (void)pvParameters;
    UBaseType_t check;
    TickType_t xLastWakeTime;
    TickType_t xCurrentTime;

    while (1) 
    {
        Serial.println(" Lane-3 is running");

        // Green on Lane 3, Red on all others
        digitalWrite(Y1, LOW);  digitalWrite(Y2, LOW);  digitalWrite(Y3, LOW);  digitalWrite(Y4, LOW);
        digitalWrite(G1, LOW);  digitalWrite(G2, LOW);  digitalWrite(G3, HIGH); digitalWrite(G4, LOW);
        digitalWrite(R1, HIGH); digitalWrite(R2, HIGH); digitalWrite(R3, LOW);  digitalWrite(R4, HIGH);

        for (int i = Delay_Tick_Array[2]; i >= 1; i--) 
        {
            displayDigit(i);
            xLastWakeTime = xTaskGetTickCount();
            xCurrentTime = xLastWakeTime;
            while ((xCurrentTime - xLastWakeTime) < xDelay) {
                xCurrentTime = xTaskGetTickCount();
            }
        }

        digitalWrite(Y3, HIGH);
        for (int i = 2; i >= 1; i--) 
        {
            displayDigit(i);
            xLastWakeTime = xTaskGetTickCount();
            xCurrentTime = xLastWakeTime;
            while ((xCurrentTime - xLastWakeTime) < xDelay) {
                xCurrentTime = xTaskGetTickCount();
            }
        }

        check = uxTaskPriorityGet(taskLane3Handle);
        if (check == 0) {
            vTaskResume(MasterTaskHandle);
            vTaskResume(taskLane1Handle);
            vTaskResume(taskLane2Handle);
            vTaskResume(taskLane3Handle);
            vTaskResume(taskLane4Handle);
            taskYIELD();
        } else {
            vTaskSuspend(taskLane3Handle);
        }
    }
}

/* ==============================================================================
 * 5. LANE 4 TASK
 * ============================================================================== */
void taskLane4(void *pvParameters) 
{
    (void)pvParameters;
    UBaseType_t check;
    TickType_t xLastWakeTime;
    TickType_t xCurrentTime;

    while (1) 
    {
        Serial.println(" Lane-4 is running");

        // Green on Lane 4, Red on all others
        digitalWrite(Y1, LOW);  digitalWrite(Y2, LOW);  digitalWrite(Y3, LOW);  digitalWrite(Y4, LOW);
        digitalWrite(G1, LOW);  digitalWrite(G2, LOW);  digitalWrite(G3, LOW);  digitalWrite(G4, HIGH);
        digitalWrite(R1, HIGH); digitalWrite(R2, HIGH); digitalWrite(R3, HIGH); digitalWrite(R4, LOW);

        for (int i = Delay_Tick_Array[3]; i >= 1; i--) 
        {
            displayDigit(i);
            xLastWakeTime = xTaskGetTickCount();
            xCurrentTime = xLastWakeTime;
            while ((xCurrentTime - xLastWakeTime) < xDelay) {
                xCurrentTime = xTaskGetTickCount();
            }
        }

        digitalWrite(Y4, HIGH);
        for (int i = 2; i >= 1; i--) 
        {
            displayDigit(i);
            xLastWakeTime = xTaskGetTickCount();
            xCurrentTime = xLastWakeTime;
            while ((xCurrentTime - xLastWakeTime) < xDelay) {
                xCurrentTime = xTaskGetTickCount();
            }
        }

        check = uxTaskPriorityGet(taskLane4Handle);
        if (check == 0) {
            vTaskResume(MasterTaskHandle);
            vTaskResume(taskLane1Handle);
            vTaskResume(taskLane2Handle);
            vTaskResume(taskLane3Handle);
            vTaskResume(taskLane4Handle);
            taskYIELD();
        } else {
            vTaskSuspend(taskLane4Handle);
        }
    }
}

/* ==============================================================================
 * 6. 7-SEGMENT DISPLAY DRIVER (Common Anode / Cathode)
 * ============================================================================== */
void displayDigit(int digit) 
{
    // Segment Patterns: 0 to 9 for segments A, B, C, D, E, F, G
    const byte segmentPatterns[10] = {
        B11000000, // 0
        B11111001, // 1
        B10100100, // 2
        B10110000, // 3
        B10011001, // 4
        B10010010, // 5
        B10000010, // 6
        B11111000, // 7
        B10000000, // 8
        B10010000  // 9
    };

    byte segments = segmentPatterns[digit % 10];
    for (int i = 0; i < 7; i++) {
        digitalWrite(SEGMENT_PINS[i], bitRead(segments, i));
    }
}
