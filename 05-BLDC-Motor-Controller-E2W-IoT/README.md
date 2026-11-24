# Electric Two-Wheeler BLDC Motor Controller & IoT Telematics System

> **Role:** Embedded Software & Firmware Developer (Industry Engineering Project)  
> **Core Technologies:** Field-Oriented Control (FOC), ARM Cortex-M4 (STM32G431RB), 3-Phase Inverter Driving, ESP32, GSM/GPRS (SIM800L), GPS (NEO-6M)  
> **Firmware Toolchains & Debug:** STM32CubeIDE, STM32 Motor Control Workbench, Motor Profiler, ST-Link V2, Digital Storage Oscilloscope (DSO), Logic Analyzer  
> **Technical Documentation:** [View Full Engineering Report (Project_Report.pdf)](./Project_Report.pdf)

---

## Project Overview

This project implements the real-time embedded firmware and telematics subsystem for an Electric Two-Wheeler (E2W) platform across two microcontrollers:

- **STM32G431RB (ARM Cortex-M4 @ 170 MHz):** Powertrain motor control firmware executing Field-Oriented Control (FOC), PWM gate actuation, throttle processing, and cruise control state management.
- **ESP32 Dual-Core SoC:** Telematics and access control firmware managing GPS coordinate streaming, cellular cloud connectivity, and dual-mode authenticated battery swapping.

---

## Core Firmware Implementation

### 1. Motor Control Firmware (STM32G431RB)

- **Field-Oriented Control (FOC) Execution:**
  - Performs Clarke and Park coordinate transforms on shunt-measured phase currents ($I_a, I_b, I_c$) to project stator flux into rotating rotor coordinates ($I_d, I_q$).
  - Runs dual decoupled discrete PI current loops: regulates $I_q$ (torque demand) while maintaining $I_d = 0$ for maximum efficiency.
  - Modulates complementary high/low-side PWM channels via Space Vector PWM (SVPWM) with programmable hardware dead-time.

- **Commutation & Operational Modes:**
  - **Hybrid Commutation:** Uses 120-degree Hall sensor interrupts for standstill startup and low speeds; switches to a Back-EMF (BEMF) state observer for high-speed sensorless operation.
  - **Throttle & Speed Regulation:** Multi-sample ADC acquisition with digital low-pass filtering and deadband rejection to eliminate throttle noise.
  - **Cruise Control State Machine:** Manages `IDLE`, `ACTIVE`, and `OVERRIDE` states with non-blocking external interrupts, featuring an instantaneous, fail-safe drive cutoff upon brake lever engagement.

---

### 2. Telematics & Security Firmware (ESP32)

- **Vehicle Tracking & Cellular Connectivity:**
  - Ingests and parses real-time GPS location and telemetry data over UART.
  - Executes a non-blocking AT command state machine over serial with the SIM800L GSM/GPRS modem for persistent cloud synchronization.

- **Dual-Mode Battery Swap Authentication:**
  - **Online Mode:** Synchronizes single-use dynamic OTP passkeys from the cloud fleet platform over HTTP/GPRS to release the battery solenoid lock.
  - **Offline Fallback Mode:** In cellular dead zones, automatically hosts a local captive Wi-Fi portal for on-site passkey entry.
  - **Anti-Tamper Lockout:** Enforces a 3-attempt failure threshold within 30 seconds to lock the solenoid latch against unauthorized tampering.

---

## Bench Verification & Vehicle Integration

- **Parameter Tuning:** Stator resistance ($R_s$), inductance ($L_s$), and back-EMF constants extracted via STM32 Motor Profiler; PI gains tuned using STM32 Motor Control Workbench.
- **Hardware Validation:** Phase current alignment, dead-time margins, and SVPWM waveforms verified with a digital storage oscilloscope.
- **Vehicle Testing:** Successfully deployed and validated on an operational electric two-wheeler prototype for acceleration response, cruise stability, brake safety cutoff, and battery swap actuation.

---

## Technical Documentation

- [Project_Report.pdf](./Project_Report.pdf): Complete 32-page engineering design report detailing mathematical derivations, hardware schematics, firmware flowcharts, and experimental bench results.
