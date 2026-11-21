# Embedded Systems & Firmware Engineering Portfolio

[![C](https://img.shields.io/badge/Language-Embedded%20C%20(C99)-blue.svg)]()
[![Target](https://img.shields.io/badge/Microcontroller-ARM%20Cortex--M%20(STM32)-red.svg)]()
[![Platform](https://img.shields.io/badge/Architecture-x86__64%20%7C%20Intel%20Xeon-purple.svg)]()
[![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-green.svg)]()
[![Publication](https://img.shields.io/badge/IEEE%20Xplore-Published-brightgreen)](https://ieeexplore.ieee.org/document/11135576)

Welcome to my embedded systems and firmware development portfolio repository. This repository showcases production-grade bare-metal firmware, real-time multitasking operating systems (FreeRTOS), low-level hardware drivers, and automated firmware validation frameworks.

---

## 🌟 Featured Projects

| # | Project | Key Domains | Core Highlights |
|---|---|---|---|
| **01** | [**FreeRTOS Real-Time Traffic Controller**](./01-FreeRTOS-RealTime-Controller/) | Real-Time OS, Multitasking | **IEEE Xplore Published.** Multi-tasking FreeRTOS firmware with priority inheritance mutexes, thread-safe queues, and zero-jitter ISR deferred context switching (`portYIELD_FROM_ISR`). |
| **02** | [**STM32 Bare-Metal Hardware Drivers**](./02-STM32-BareMetal-Firmware/) | Bare-Metal C, ARM Cortex-M | Direct register-level programming (`-nostdlib`) on STM32F103 (Blue Pill). Features custom startup vector table, SysTick low-power WFI sleep, Full-Duplex SPI, lock-free Ring Buffer, and **Circular DMA Ping-Pong streaming**. |
| **03** | [**Firmware Test Automation & KPI Suite**](./03-Firmware-Test-Automation/) | Python, CI/CD, Verification | Automated hardware-in-the-loop (HIL) test harness and KPI regression framework. Generates real-time HTML test execution dashboards and latency metrics. |

---

## 📑 Technical Architecture Summaries

### 1. [FreeRTOS Real-Time Controller (IEEE Publication)](./01-FreeRTOS-RealTime-Controller/)
- **Publication:** *Intelligent Real-Time Traffic Signal System using FreeRTOS* ([IEEE Xplore](https://ieeexplore.ieee.org/document/11135576)).
- **Key Achievements:**
  - Designed deterministic priority tiers (Sensor Ingestion Task, Timing Algorithm Task, Actuation Task).
  - Resolved potential **Priority Inversion** on shared intersection states using **Mutexes with Priority Inheritance**.
  - Tuned task stack allocations in SRAM using `uxTaskGetStackHighWaterMark()` and canary overflow detection.

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

## 📄 Project Reports & Technical Documentation

All technical documentation, research papers, and academic reports are organized in the [**`reports/`**](./reports/) directory:

- 📄 [**Minor Project Technical Report**](./reports/23MRE005_MINOR_PROJECT_REPORT.pdf) — Embedded system architecture, hardware interfacing, and real-time controller design.
- 📄 [**Advanced Digital System Design (ALU Report)**](./reports/ADSD_ALU.pdf) — Silicon-level Arithmetic Logic Unit design, combinational/sequential logic, and hardware verification.
- 📄 [**Digital Design Verification Report**](./reports/23MRE005_TUT5_Pnd.pdf) — Simulation waveforms, timing constraints, and physical verification.
- 📄 [**IEEE Research Paper:** *Intelligent Real-Time Traffic Signal System using FreeRTOS*](https://ieeexplore.ieee.org/document/11135576) (IEEE Xplore).

---

## 🛠️ Core Technical Skills Matrix

- **Programming Languages:** Embedded C (C99/C11, volatile pointers, bitwise operations, memory layout), Python (Automation, PyTest).
- **Architectures:** ARM Cortex-M3/M4 (STM32, Blue Pill), x86_64 Platforms (Intel Xeon, PCH, Boot Guard, UEFI).
- **RTOS & Systems:** FreeRTOS (Tasks, Queues, Semaphores, Mutexes, Event Groups), Bare-Metal Bring-up, Linker Scripts (`linker.ld`), Startup Code (`startup.c`).
- **Communication Protocols:** SPI, UART, I2C, CAN Bus, DMA, PCIe Gen 5, SerDes (Equalization, CDR, Link Training).
- **Lab & Debug Equipment:** Logic Analyzer (Saleae), Oscilloscope, JTAG / SWD (ST-Link, OpenOCD, GDB), Intel ITP / OpenIPC.
- **Tools & Build Systems:** GNU Arm Embedded Toolchain (`arm-none-eabi-gcc`), Makefiles, Git, VS Code.
