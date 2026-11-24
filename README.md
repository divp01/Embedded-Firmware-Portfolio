# Embedded Systems & Firmware Engineering Portfolio

[![C](https://img.shields.io/badge/Language-Embedded%20C%20(C99)-blue.svg)]()
[![Target](https://img.shields.io/badge/Microcontroller-ARM%20Cortex--M%20(STM32)-red.svg)]()
[![Platform](https://img.shields.io/badge/Architecture-x86__64%20%7C%20Intel%20Xeon-purple.svg)]()
[![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-green.svg)]()
[![Publication](https://img.shields.io/badge/IEEE%20Xplore-Published-brightgreen)](https://ieeexplore.ieee.org/document/11135576)

Welcome to my embedded systems and firmware development portfolio repository. This repository showcases production-grade bare-metal firmware, real-time multitasking operating systems (FreeRTOS), low-level hardware drivers, and automated firmware validation frameworks.

---

## Featured Projects

| # | Project | Key Domains | Core Highlights |
|---|---|---|---|
| **01** | [**FreeRTOS - RTTCS**](./01-FreeRTOS-RTTCS/) | Real-Time OS, Multitasking | **IEEE Xplore Published.** Multi-lane FreeRTOS firmware featuring dynamic priority shifting (`vTaskPrioritySet`), density-driven green intervals, and cooperative task scheduling on ATmega2560. <br/>• [View Execution Flowchart](./01-FreeRTOS-RTTCS/#system-execution-flowchart) • [Project Report](./01-FreeRTOS-RTTCS/Project_Report.pdf) |
| **02** | [**STM32 Bare-Metal Hardware Drivers**](./02-STM32-BareMetal-Firmware/) | Bare-Metal C, ARM Cortex-M | Direct register-level programming (`-nostdlib`) on STM32F103 (Blue Pill). Features custom startup vector table, SysTick low-power WFI sleep, Full-Duplex SPI, lock-free Ring Buffer, and **Circular DMA Ping-Pong streaming**. |
| **03** | [**Firmware Test Automation & KPI Suite**](./03-Firmware-Test-Automation/) | Python, CI/CD, Verification | Automated hardware-in-the-loop (HIL) test harness and KPI regression framework. Generates real-time HTML test execution dashboards and latency metrics. |

---

## Technical Architecture Summaries

### 1. [FreeRTOS - RTTCS (IEEE Publication)](./01-FreeRTOS-RTTCS/)
- **Publication:** *Intelligent Real-Time Traffic Signal System using FreeRTOS* ([IEEE Xplore](https://ieeexplore.ieee.org/document/11135576)).
- **Flowchart Diagram:** [View Task Execution Flowchart](./01-FreeRTOS-RTTCS/#system-execution-flowchart)
- **Key Achievements:**
  - Implemented dynamic runtime priority reassignment (`vTaskPrioritySet()`) based on IR sensor density detection across 4 lanes.
  - Designed cooperative priority handoff architecture using `vTaskSuspend()` and `vTaskResume()` cycle synchronization.
  - Dynamically modulated green signal duty intervals (7 ticks for high congestion vs. 5 ticks for normal flow).
  - Hardware validated using Digilent Analog Discovery 2 logic analyzer and pattern generator.

---

### 2. [STM32 Bare-Metal Firmware Labs](./02-STM32-BareMetal-Firmware/)
- **Target:** STM32F103C8T6 (ARM Cortex-M3 @ 72 MHz).
- **Core Implementations:**
  - **Lesson 1 ([01_systick_blink.c](./02-STM32-BareMetal-Firmware/app/01_systick_blink.c)):** Hardware SysTick timer exception #15 waking the core from low-power `WFI` sleep every 500 ms (Footprint: 432 bytes).
  - **Lesson 2 ([02_spi_ringbuffer.c](./02-STM32-BareMetal-Firmware/app/02_spi_ringbuffer.c)):** High-speed streaming data ingestion via SPI1 RXNE interrupt coupled with a power-of-2 lock-free circular FIFO (`(tail + 1) & 127`).
  - **Lesson 3 ([03_spi_dma_pingpong.c](./02-STM32-BareMetal-Firmware/app/03_spi_dma_pingpong.c)):** Zero-copy continuous DMA streaming using **DMA1 Channel 2** in hardware `CIRCULAR` mode with Half-Transfer (`HT`) and Transfer-Complete (`TC`) interrupt ping-ponging.

---

### 3. [Firmware Test Automation & KPI Framework](./03-Firmware-Test-Automation/)
- Automated regression suite validating device firmware under load.
- Measures transaction latency, throughput, and error recovery metrics.
- Emits real-time JSON summaries and HTML dashboard reports.

---

## Project Reports & Technical Documentation

All major engineering reports, IEEE research papers, and technical project documentations are organized in the [**`reports/`**](./reports/) directory:

- [**IEEE Research Paper (RTITCS)**](https://ieeexplore.ieee.org/document/11135576) — *Intelligent Real-Time Traffic Signal System using FreeRTOS* (Published on [IEEE Xplore](https://ieeexplore.ieee.org/document/11135576)).
- [**Major Project Report**](./reports/19BEE032_MAJOR%20PROJECT.pdf) — Comprehensive Capstone Engineering Project Report.
- [**Minor Project Technical Report (RTTCS)**](./01-FreeRTOS-RTTCS/Project_Report.pdf) — Real-time embedded system architecture and firmware design.
- [**SPI Protocol & Design (PnD) Report**](./reports/PnD_SPI.pdf) — Physical design, timing waveforms, and protocol verification of SPI bus interfacing.
- [**Embedded Systems Project (ESP) Report**](./reports/23mre005_ESP_REPORT_SP.pdf) — Hardware peripheral interfacing and microcontroller integration.
- [**Advanced Digital System Design: ALU**](./reports/ADSD_ALU.pdf) — Silicon-level Arithmetic Logic Unit design and verification.
- [**ADSD Special Project Report**](./reports/23MRE005_ADSD_SP_REPORT.pdf) — Advanced digital logic synthesis and simulation analysis.
- [**Digital Design Verification Report**](./reports/23MRE005_TUT5_Pnd.pdf) — Hardware timing constraints and simulation waveforms.
- [**M.Tech Embedded Systems Thesis**](./reports/Nirma_ES_23MRE005_Divyani_Patel.pdf) — Nirma University Embedded Systems Engineering Master's Thesis.

---

## Project Presentations & Review Slides

Official engineering presentation decks are available in the [**`presentations/`**](./presentations/) directory:

- [**Major Project Review 1 Presentation**](./presentations/23MRE005_Major_Project_Review-1.pptx) — Architecture, block diagrams, and milestone review.
- [**Minor Project Final Review Presentation**](./presentations/23MRE005_Minor_Final.pptx) — Real-time control system final defense slides.

---

## Certifications & Credentials

Verified certificates are archived in the [**`certificates/`**](./certificates/) directory:

- [**Problem Solving through Programming in C (NPTEL - IIT Kharagpur, Score: 83%)**](./certificates/Problem%20Solving%20through%20Programming%20in%20C.jpg)
- [**IoT & Embedded Systems Technical Certification**](./certificates/IOT_Webinar.jpg)
- [**Virtual Summer School on Recent Embedded Trends**](./certificates/Virtual%20Summer%20School%20on%20Recent%20Trends.pdf)

---

## Core Technical Skills Matrix

- **Programming Languages:** Embedded C (C99/C11, volatile pointers, bitwise operations, memory layout), Python (Automation, PyTest).
- **Architectures:** ARM Cortex-M3/M4 (STM32, Blue Pill), x86_64 Platforms (Intel Xeon, PCH, Boot Guard, UEFI).
- **RTOS & Systems:** FreeRTOS (Tasks, Queues, Semaphores, Mutexes, Event Groups), Bare-Metal Bring-up, Linker Scripts (`linker.ld`), Startup Code (`startup.c`).
- **Communication Protocols:** SPI, UART, I2C, CAN Bus, DMA, PCIe Gen 5, SerDes (Equalization, CDR, Link Training).
- **Lab & Debug Equipment:** Logic Analyzer (Saleae), Oscilloscope, JTAG / SWD (ST-Link, OpenOCD, GDB), Intel ITP / OpenIPC.
- **Tools & Build Systems:** GNU Arm Embedded Toolchain (`arm-none-eabi-gcc`), Makefiles, Git, VS Code.
