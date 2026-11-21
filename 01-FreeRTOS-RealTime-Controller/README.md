# Intelligent Real-Time Traffic Signal Controller using FreeRTOS

[![Publication](https://img.shields.io/badge/IEEE%20Xplore-Published-blue)](https://ieeexplore.ieee.org/document/11135576)
[![Language](https://img.shields.io/badge/Language-Embedded%20C-orange.svg)]()
[![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-green.svg)]()
[![Target](https://img.shields.io/badge/Target-ARM%20Cortex--M-red.svg)]()

> **Peer-Reviewed Research:** Published in **IEEE Xplore** (08/2025).  
> **Paper Title:** *Intelligent Real-Time Traffic Signal System using FreeRTOS*  
> **Direct IEEE Link:** [https://ieeexplore.ieee.org/document/11135576](https://ieeexplore.ieee.org/document/11135576)

---

## 🚦 Project Overview

This project implements a deterministic, multi-tasking traffic control system in **Embedded C** utilizing **FreeRTOS** on an ARM Cortex-M architecture.

Unlike fixed-interval traffic controllers that cause unnecessary congestion and idle vehicle delays, this system dynamically calculates real-time traffic density using multi-channel sensor arrays and continuously adjusts signal timing to maximize throughput.

---

## ⚙️ Core Architecture & FreeRTOS Design

```
   ┌────────────────────────────────────────────────────────┐
   │             HARDWARE INTERRUPTS (IR SENSORS)           │
   └───────────────────────────┬────────────────────────────┘
                               │ xQueueSendFromISR() + portYIELD_FROM_ISR()
                               ▼
   ┌────────────────────────────────────────────────────────┐
   │           SENSOR INGESTION TASK (High Priority)        │
   │  - Debounces & samples multi-channel vehicle pulses    │
   │  - Packages sensor readings into structured event      │
   └───────────────────────────┬────────────────────────────┘
                               │ Thread-Safe Queue (xQueueSend)
                               ▼
   ┌────────────────────────────────────────────────────────┐
   │           TIMING & ALGORITHM TASK (Med Priority)       │
   │  - Calculates dynamic green light intervals            │
   │  - Acquired state Mutex (with Priority Inheritance)    │
   └───────────────────────────┬────────────────────────────┘
                               │ State Machine Event
                               ▼
   ┌────────────────────────────────────────────────────────┐
   │           SIGNAL ACTUATION & LOGGING (Low Priority)    │
   │  - Drives LED signal heads via GPIO / PWM              │
   │  - Transmits real-time density telemetry over UART     │
   └────────────────────────────────────────────────────────┘
```

---

## 🔑 Key Engineering Challenges & Solutions

### 1. Deterministic Interrupt Handoff (Zero-Jitter Ingestion)
- **Problem:** High-frequency vehicle sensor pulses must be captured immediately without stalling the CPU in long ISRs.
- **Solution:** Implemented the **Top Half / Bottom Half** deferred processing pattern:
  - The sensor ISR pushes timestamped event packets into a FreeRTOS Queue via **`xQueueSendFromISR()`**.
  - Calls **`portYIELD_FROM_ISR()`** at the end of the ISR to force an immediate context switch directly into the waiting sensor task.

### 2. Preventing Priority Inversion on Shared Traffic State
- **Problem:** The high-priority Emergency Override task and low-priority telemetry logging task share the global intersection state. A medium-priority task could preempt the low task while it holds the resource, inadvertently starving the emergency task.
- **Solution:** Strictly used **FreeRTOS Mutexes with Priority Inheritance** instead of raw binary semaphores. When the high task requests the mutex, FreeRTOS temporarily elevates the low task's priority, eliminating inversion.

### 3. Stack Profiling & Memory Safety in SRAM
- Every task was allocated an optimized private stack.
- Enabled **`configCHECK_FOR_STACK_OVERFLOW = 2`** to catch canary byte corruption.
- Monitored stack margins during peak vehicle burst simulations using **`uxTaskGetStackHighWaterMark()`**.

---

## 📂 Source Code Structure

- [src/main.c](file:///d:/Development/Firmware/Embedded-Firmware-Portfolio/01-FreeRTOS-RealTime-Controller/src/main.c): System initialization, FreeRTOS task creation, and scheduler start.
- [src/traffic_controller.h](file:///d:/Development/Firmware/Embedded-Firmware-Portfolio/01-FreeRTOS-RealTime-Controller/src/traffic_controller.h): Intersection state definitions, queue event structures, and API contracts.
- `docs/`: Dedicated folder for the IEEE publication PDF report.
