# Intelligent Real-Time Traffic Signal System using FreeRTOS

[![Publication](https://img.shields.io/badge/IEEE%20Xplore-Published-blue)](https://ieeexplore.ieee.org/document/11135576)
[![Language](https://img.shields.io/badge/Language-Embedded%20C%2FC%2B%2B-orange.svg)]()
[![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-green.svg)]()
[![Target](https://img.shields.io/badge/Hardware-Arduino%20Mega%202560-red.svg)]()
[![Validation](https://img.shields.io/badge/Verified-Analog%20Discovery%202-purple.svg)]()

> **Peer-Reviewed Research:** Published in **IEEE Xplore** (08/2025).  
> **Course:** 6EC191 Minor Project Report, Department of Electronics & Communication Engineering, Nirma University.  
> **Author:** Divyani Patel (23MRE005)  
> **Direct IEEE Link:** [https://ieeexplore.ieee.org/document/11135576](https://ieeexplore.ieee.org/document/11135576)  
> **Full PDF Report:** [View Report PDF in `reports/23MRE005_MINOR_PROJECT_REPORT.pdf`](../reports/23MRE005_MINOR_PROJECT_REPORT.pdf)

---

## 🚦 Project Overview

Urban traffic congestion in Indian cities causes severe economic losses and air pollution due to inflexible, fixed-timing traffic light controllers. This project designs and implements an **Intelligent Traffic Control System (ITCS)** on an **ATmega2560 (Arduino Mega 2560 @ 16 MHz)** running **FreeRTOS**.

The system dynamically detects vehicle density across 4 lanes using high/low IR sensor pairs and automatically reconfigures signal timings and **task execution priorities in real-time** using FreeRTOS scheduling APIs.

---

## 🛠️ Hardware & Software Architecture

### Hardware Components:
- **Microcontroller:** Arduino Mega 2560 (ATmega2560 @ 16 MHz, 256 KB Flash, 8 KB SRAM, 54 Digital I/O pins).
- **Sensors:** 8 IR Proximity Sensors (2 per lane: High-Density `H` and Low-Density `L`).
- **Actuators:** 12 Traffic LEDs (Red, Yellow, Green for Lanes 1–4) + 7-Segment Countdown Display.
- **Test & Validation Instrument:** **Digilent Analog Discovery 2 Kit** (USB Oscilloscope & Logic Analyzer via WaveForms software).

### Software Architecture:
- **Operating System:** FreeRTOS v10.6.2.
- **Scheduling Model:** Cooperative & Priority-driven task management (`configUSE_PREEMPTION = 0`, `configMAX_PRIORITIES = 6`).

---

## ⚙️ FreeRTOS Task Structure & Priority Allocation

```
                   ┌────────────────────────────────────────┐
                   │    MasterTask_Sensor (Priority 4)      │
                   │  - Samples 8 IR Sensors (H1..H4, L1..L4)│
                   │  - Computes Density Array              │
                   │  - Dynamically runs vTaskPrioritySet() │
                   └──────────────────┬─────────────────────┘
                                      │ Dynamic Priority Elevation
                                      ▼
     ┌──────────────────┬──────────────────┬──────────────────┬──────────────────┐
     │  taskLane1       │  taskLane2       │  taskLane3       │  taskLane4       │
     │  - Green/Yellow  │  - Green/Yellow  │  - Green/Yellow  │  - Green/Yellow  │
     │  - 7-Segment Cnt │  - 7-Segment Cnt │  - 7-Segment Cnt │  - 7-Segment Cnt │
     │  - vTaskSuspend  │  - vTaskSuspend  │  - vTaskSuspend  │  - vTaskSuspend  │
     └──────────────────┴──────────────────┴──────────────────┴──────────────────┘
```

### Dynamic Priority Algorithm:
- When both `H` (High) and `L` (Low) sensors detect vehicles on a lane, that lane is assigned **High Priority** and given a **7-tick green light interval**.
- Normal density lanes receive a **5-tick green light interval**.
- The `MasterTask` executes `vTaskPrioritySet()` in real-time, ordering lane execution from highest to lowest density:
  $$\text{Priority Ordering: Lane with Max Density} \to \dots \to \text{Lane with Min Density}$$
- After the lowest priority task completes (`uxTaskPriorityGet() == 0`), it calls `vTaskResume(MasterTaskHandle)` to initiate the next adaptive cycle.

---

## 🔬 Experimental Validation (Analog Discovery 2)

The system was physically probed and validated using the **Digilent Analog Discovery 2 USB Logic Analyzer**:
- **High-Density Lane (H=1, L=1):** Measured green signal active duration = **9.07 seconds**.
- **Low-Density Lane (H=1, L=0):** Measured green signal active duration = **7.3 seconds**.
- Verified zero task deadlocks and exact priority scheduling through WaveForms digital pattern generator and logic analyzer channels.

---

## 📂 Source Code

- [src/Traffic_Controller_4Lane.ino](./src/Traffic_Controller_4Lane.ino): Complete, verified 4-lane adaptive traffic signal firmware from thesis Appendix.
- [reports/23MRE005_MINOR_PROJECT_REPORT.pdf](../reports/23MRE005_MINOR_PROJECT_REPORT.pdf): Full 41-page thesis report.
