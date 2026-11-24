# Implementation of SPI Bus Protocol (STM32 to Arduino)

> **Course:** Processor Architecture & Design [6EC202], Department of Electronics & Communication Engineering, Nirma University  
> **Author:** Divyani Patel (23MRE005)  
> **Submitted to:** Dr. Dhaval Shah  
> **Project Report / Presentation:** [View Technical Report (Project_Report.pdf)](./Project_Report.pdf)

---

## Project Overview

This academic project implements a hardware SPI bus communication link between an STM32F103C8T6 microcontroller operating as the SPI Master and an Arduino Uno (ATmega328P) operating as the SPI Slave. 

Because the STM32 operates at 3.3V logic and the Arduino operates at 5V logic, the bus lines are bridged through a 4-channel bidirectional logic level shifter to ensure safe and reliable signal transmission.

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

## Hardware Verification & Results

The physical system was connected on a breadboard using an ST-Link V2 debugger and USB serial monitoring interface:
- Master transmitted length-prefixed data strings across the isolated level shifter.
- The Arduino slave triggered interrupt-driven reception on SCK edges while SS remained asserted low.
- Successful message decoding and length verification were verified on the serial console (COM5) with zero frame drops.

---

## Project Documentation

- [Project_Report.pdf](./Project_Report.pdf): Complete technical slides and hardware documentation.
