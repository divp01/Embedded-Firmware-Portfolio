# Implementation of SPI Bus Protocol (STM32 to Arduino)

> **Core Technologies:** Embedded C, ARM Cortex-M3 (STM32F103C8T6), ATmega328P, SPI Bus Protocol, Bidirectional Voltage Translation  
> **Bench Tools & Debug:** ST-Link V2 Debugger, Keil uVision MDK-ARM, Serial Monitor (COM5), Hardware Bench Setup  
> **Engineering Documentation:** [View Technical Architecture Report (Project_Report.pdf)](./Project_Report.pdf)

---

## Project Overview

This firmware project implements a robust, hardware-validated SPI (Serial Peripheral Interface) communication protocol bridging an **ARM Cortex-M3 microcontroller (STM32F103C8T6)** operating as the SPI Master and an **AVR microcontroller (Arduino Uno / ATmega328P)** operating as the SPI Slave.

Because the STM32 operates at 3.3V logic and the Arduino operates at 5V logic, the high-speed bus lines are bridged through a 4-channel bidirectional logic level shifter to prevent IO overvoltage and maintain signal integrity.

---

## Hardware Interfacing & Pin Mapping

| Signal | STM32F103 (Master - 3.3V) | Level Shifter | Arduino Uno (Slave - 5V) |
|---|---|---|---|
| **SCK (Serial Clock)** | PA5 | LV1 <-> HV1 | Pin 13 |
| **MOSI (Master Out Slave In)** | PA7 | LV2 <-> HV2 | Pin 11 |
| **NSS / SS (Slave Select)** | PA4 | LV3 <-> HV3 | Pin 10 |
| **VCC & GND** | 3.3V & GND | LV / HV & GND | 5V & GND |

---

## Bus Parameters & Protocol Specifications

- **Operating Mode:** Transmit-only Master, Receive-only Slave
- **Clock Configuration:** SCLK = 1 MHz, generated via a prescaler of 8 from the 8 MHz core system clock (HSI)
- **Clock Phase & Polarity:** SPI Mode 0 (CPOL = 0, CPHA = 0)
- **Data Frame Format:** 8-bit data frame (`DFF = 0`), MSB first
- **Slave Management:** Hardware NSS output management (`SSM = 0`)
- **Frame Protocol:** Dynamic length handling. Since the slave does not know incoming buffer sizes in advance, the master transmits a 1-byte length prefix immediately before the payload string.

---

## Hardware Verification & Bench Results

The physical system was connected on a breadboard using an ST-Link V2 debugger and USB serial monitoring interface:
- Master transmitted length-prefixed data strings across the isolated level shifter.
- The Arduino slave triggered interrupt-driven reception on SCK edges while SS remained asserted low.
- Successful message decoding and length verification were verified on the serial console (COM5) with zero frame drops.

---

## Firmware Architecture & Low-Level Design

From a low-level peripheral driver and firmware architecture standpoint:

- **Register-Level Peripheral Control:**
  - **STM32 (Master):** Configures `SPI_CR1` (Clock polarity `CPOL=0`, phase `CPHA=0`, baud prescaler `BR[2:0]=010` for `/8`, master mode `MSTR=1`), and enables hardware slave select output via `SSOE=1` in `SPI_CR2`.
  - **AVR (Slave):** Enables the SPI peripheral and interrupt generation directly via the `SPCR` control register (`SPE=1`, `SPIE=1`), setting MISO as output while maintaining SCK, MOSI, and SS in high-impedance input mode.

- **Interrupt-Driven Reception & Low Latency:**
  - Slave reception is handled asynchronously by the `SPI_STC_vect` (Serial Transfer Complete) interrupt handler.
  - The ISR reads data register `SPDR` in a single instruction cycle, preventing Hardware Overrun (`OVR`) conditions without stalling the main processor thread.

- **Bus Timing & Voltage Translation Constraints:**
  - Interfacing 3.3V LVCMOS to 5V TTL requires active logic level shifting. SCLK is held at 1 MHz (1 us period) to accommodate the propagation delay (~15 ns) and capacitive load ($C_L$) across the bidirectional MOSFET level shifter channels.
  - Hardware NSS assertion automatically frames the transaction; deassertion triggers frame processing without needing external handshaking pins.

- **Defensive Framing & Memory Safety:**
  - Length-prefixed dynamic framing ensures the receiving slave validates message bounds (`pos < sizeof(dataBuff) - 1`) prior to buffer indexing, preventing memory corruption or stack smashes from partial or noisy bus transactions.

---

## Key Engineering Competencies Demonstrated

- **Heterogeneous Architecture Interfacing:** Bridged 32-bit ARM Cortex-M and 8-bit AVR microcontrollers across mixed voltage levels (3.3V / 5V).
- **Bus Timing & Prescaling:** Derived 1 MHz SCLK from 8 MHz core clock via hardware prescaler configuration while respecting SPI Mode 0 setup and hold constraints.
- **Robust Framing Protocol:** Implemented length-prefixed framing to eliminate buffer overrun hazards without software polling overhead.
- **Hardware-in-the-Loop Validation:** Verified transmission end-to-end using dedicated hardware debuggers (ST-Link V2) and real-time serial telemetry.

---

## Technical Documentation

- [Project_Report.pdf](./Project_Report.pdf): Engineering presentation and hardware architecture documentation.
