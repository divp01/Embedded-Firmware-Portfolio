# Embedded Systems & Firmware Engineering Portfolio

[![C](https://img.shields.io/badge/Language-Embedded%20C%20(C99)-blue.svg)]()
[![Target](https://img.shields.io/badge/Microcontroller-ATmega2560-red.svg)]()
[![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-green.svg)]()
[![Publication](https://img.shields.io/badge/IEEE%20Xplore-Published-brightgreen)](https://ieeexplore.ieee.org/document/11135576)

Welcome to my Embedded Systems & Firmware Engineering Portfolio. This repository showcases production-grade embedded firmware, real-time operating systems (FreeRTOS), hardware communication bus protocols (SPI, UART, I2C), low-level hardware register programming, and automated hardware bench verification.

---

## Featured Projects

| # | Project | Key Domains | Core Highlights |
|---|---|---|---|
| **01** | [**FreeRTOS Real-Time Traffic Controller**](./01-FreeRTOS-RTTCS/) | Real-Time OS, Multitasking | **IEEE Xplore Published.** Multi-lane FreeRTOS firmware featuring dynamic priority shifting (`vTaskPrioritySet`), density-driven green intervals, and cooperative task scheduling on ATmega2560. <br/>• [View Execution Flowchart](./01-FreeRTOS-RTTCS/#system-execution-flowchart) • [Project Report](./01-FreeRTOS-RTTCS/Project_Report.pdf) |
| **02** | [**SPI Master-Slave Bus Protocol**](./02-SPI-Master-Slave-Protocol/) | Bus Protocols, Interfacing | STM32F103 (Master) to Arduino Uno (Slave) SPI communication via 4-channel logic level shifter. Features length-prefixed frame protocol, 1 MHz clocking, and hardware verification. <br/>• [Project Report](./02-SPI-Master-Slave-Protocol/Project_Report.pdf) |
| **03** | [**Embedded C++ Hardware Abstraction**](./03-Embedded-CPP-Hardware-Abstraction/) | Embedded C++, ARM Cortex-M | Bare-metal object-oriented peripheral driver on STM32F103. Features hardware encapsulation, direct register bitmasking, and zero-overhead C++ abstractions. <br/>• [Project Report](./03-Embedded-CPP-Hardware-Abstraction/Project_Report.pdf) |
| **04** | [**8-Bit ALU & FPGA Verification**](./04-FPGA-8Bit-ALU-Verification/) | Verilog HDL, FPGA, HIL Testing | Synthesizable 8-bit ALU on Altera Cyclone II FPGA. Validated via closed-loop Hardware-in-the-Loop testbench with dual Analog Discovery 2 instruments. <br/>• [Project Report](./04-FPGA-8Bit-ALU-Verification/Project_Report.pdf) |
| **05** | [**E2W BLDC Motor Controller & Telematics**](./05-BLDC-Motor-Controller-E2W-IoT/) | Motor Control (FOC), Embedded Software | **Role: Embedded Software Developer.** STM32G431RB Field-Oriented Control (FOC) firmware with Clarke/Park math, SVPWM, cruise control state machine, and ESP32 cellular telematics / battery swap authentication. <br/>• [Project Report](./05-BLDC-Motor-Controller-E2W-IoT/Project_Report.pdf) |

---

## Technical Architecture Summaries

### 1. [FreeRTOS Real-Time Controller (IEEE Publication)](./01-FreeRTOS-RTTCS/)
- **Publication:** *Intelligent Real-Time Traffic Signal System using FreeRTOS* ([IEEE Xplore](https://ieeexplore.ieee.org/document/11135576)).
- **Flowchart Diagram:** [View Task Execution Flowchart](./01-FreeRTOS-RTTCS/#system-execution-flowchart)
- **Key Achievements:**
  - Implemented dynamic runtime priority reassignment (`vTaskPrioritySet()`) based on IR sensor density detection across 4 lanes.
  - Designed cooperative priority handoff architecture using `vTaskSuspend()` and `vTaskResume()` cycle synchronization.
  - Dynamically modulated green signal duty intervals (7 ticks for high congestion vs. 5 ticks for normal flow).
  - Hardware validated using Digilent Analog Discovery 2 logic analyzer and pattern generator.

---

### 2. [SPI Master-Slave Bus Protocol](./02-SPI-Master-Slave-Protocol/)
- **Focus Domains:** Hardware Bus Protocols, Cross-Voltage Logic Translation, Low-Level Driver Architecture.
- **Hardware Architecture:** STM32F103 (3.3V Master) interfaced to Arduino Uno (5V Slave) across a 4-channel bidirectional logic level shifter.
- **Bus Specifications:** 1 MHz SCLK (Prescaler 8 from 8 MHz core), Mode 0 (CPOL=0, CPHA=0), 8-bit MSB-first frame format, hardware NSS management.
- **Key Achievements:**
  - Implemented length-prefixed dynamic transmission protocol preventing buffer overruns on the receiver.
  - Configured hardware GPIO alternate functions and clock gating via STM32 registers and HAL drivers.
  - Physically validated on breadboard setup with ST-Link V2 and Arduino serial monitoring.

---

### 3. [Embedded C++ Hardware Abstraction](./03-Embedded-CPP-Hardware-Abstraction/)
- **Focus Domains:** Embedded C++, Bare-Metal ARM Cortex-M, Object-Oriented Hardware Abstraction.
- **Target Hardware:** STM32F103C8T6 (ARM Cortex-M3 @ 72 MHz).
- **Key Achievements:**
  - Encapsulated peripheral state and configuration logic within a lightweight C++ driver class without dynamic memory allocation or vtable overhead.
  - Implemented direct memory-mapped register manipulation for bus clock gating (`RCC_APB2ENR`), pin direction (`GPIOC_CRH`), and state driving (`GPIOC_ODR`).
  - Validated deterministic state transitions and scoping on physical breadboard hardware using Keil MDK-ARM.

---

### 4. [8-Bit ALU & FPGA Verification](./04-FPGA-8Bit-ALU-Verification/)
- **Focus Domains:** Verilog HDL, RTL Design, FPGA Synthesis, Hardware-in-the-Loop (HIL) Verification.
- **Target Hardware:** Altera (Intel) Cyclone II EP2C35F672C6 on Terasic DE2 Development Board.
- **Key Achievements:**
  - Implemented an 8-bit combinational ALU in synthesizable Verilog HDL supporting 8 arithmetic and logic operations with 16-bit product retention and status flags (Zero, Carry).
  - Architected a closed-loop Hardware-in-the-Loop (HIL) automated test harness utilizing dual Digilent Analog Discovery 2 units (one as digital pattern generator, one as logic analyzer at 50 MHz).
  - Validated full instruction set truth table and verified zero timing glitches under real silicon execution.

---

### 5. [E2W BLDC Motor Controller & IoT Telematics](./05-BLDC-Motor-Controller-E2W-IoT/)
- **Role:** Embedded Software & Firmware Developer (Industry Engineering Project).
- **Focus Domains:** Motor Control Firmware, Field-Oriented Control (FOC), Real-Time State Machines, Cellular Telematics, Embedded Security.
- **Target Hardware:** STM32G431RB (ARM Cortex-M4 @ 170 MHz), X-NUCLEO-IHM16M1 (3-Phase Inverter Driver), ESP32, GSM SIM800L, GPS NEO-6M.
- **Key Software Achievements:**
  - Implemented Field-Oriented Control (FOC) firmware algorithms: Clarke/Park vector transforms, decoupled $I_d$/$I_q$ PI current regulation loops, and Space Vector PWM (SVPWM) duty-cycle modulation.
  - Architected hybrid commutation state machine: Hall sensor feedback for low-speed/standstill startup and Back-EMF (BEMF) state observer for high-speed sensorless operation.
  - Developed ADC throttle digital filtering pipeline and interrupt-driven cruise control state machine with instantaneous, fail-safe brake override.
  - Engineered ESP32 telematics firmware: cellular modem communication (SIM800L), real-time GPS location tracking (NEO-6M), and dual-mode battery swap authentication (Cloud HTTP API with local captive Wi-Fi fallback and 3-attempt lockout security).
  - Validated and tuned control loops on hardware bench using STM32 Motor Control Workbench and on-vehicle prototype.

---

## Research Publications & Technical Documentation

- **IEEE Research Paper (RTITCS):** *Intelligent Real-Time Traffic Signal System using FreeRTOS* — Published on [IEEE Xplore (Direct Link)](https://ieeexplore.ieee.org/document/11135576).
- **Engineering Project Reports:** Each featured project includes its complete technical design report co-located directly within its repository folder:
  - [Project 01: FreeRTOS Real-Time Controller Report](./01-FreeRTOS-RTTCS/Project_Report.pdf)
  - [Project 02: SPI Master-Slave Bus Protocol Report](./02-SPI-Master-Slave-Protocol/Project_Report.pdf)
  - [Project 03: Embedded C++ Hardware Abstraction Report](./03-Embedded-CPP-Hardware-Abstraction/Project_Report.pdf)
  - [Project 04: 8-Bit ALU FPGA Verification Report](./04-FPGA-8Bit-ALU-Verification/Project_Report.pdf)
  - [Project 05: E2W BLDC Motor Controller & Telematics Report](./05-BLDC-Motor-Controller-E2W-IoT/Project_Report.pdf)

---

## Verified Certifications & Credentials

- [**Problem Solving through Programming in C — NPTEL (IIT Kharagpur)**](./certificates/Problem%20Solving%20through%20Programming%20in%20C.jpg)  
  *Score: 83% (Elite)* — Rigorous proctored certification covering low-level C programming, pointer arithmetic, memory layout, and data structures. Complete credential details can be viewed in the [certificates directory](./certificates/).

---

## Core Technical Skills Matrix

- **Programming Languages:** Embedded C (C99/C11, volatile pointers, bitwise operations, memory layout), Embedded C++, Python (Automation, Test Scripting), Verilog HDL.
- **Architectures:** ARM Cortex-M4/M3 (STM32G431, STM32F103), Xtensa Dual-Core (ESP32), AVR (ATmega2560), Altera / Intel FPGA (Cyclone II).
- **Motor Control & Power:** Field-Oriented Control (FOC), Space Vector PWM (SVPWM), Clarke/Park Transforms, Back-EMF Observers, 3-Phase Inverter Driving.
- **RTOS & Systems:** FreeRTOS (Tasks, Queues, Semaphores, Mutexes, Event Groups), Bare-Metal Hardware Bring-up, Direct Register Manipulation.
- **Communication Protocols:** SPI, UART, I2C, Cellular AT Commands (SIM800L), GPS telemetry, GPIO alternate functions.
- **Lab & Debug Equipment:** Digilent Analog Discovery 2 (Logic Analyzer & Pattern Generator), Digital Storage Oscilloscope (DSO), JTAG / SWD (ST-Link V2, Keil uVision, OpenOCD, GDB).
- **Tools & Build Systems:** STM32CubeIDE, STM32 Motor Control Workbench, Motor Profiler, GNU Arm Embedded Toolchain (`arm-none-eabi-gcc`), Keil MDK-ARM v5, Quartus II, Git.
