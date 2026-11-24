# Embedded C++ Hardware Abstraction & Peripheral Driver

> **Core Technologies:** Embedded C++, ARM Cortex-M3 (STM32F103C8T6), Object-Oriented Firmware Design, Direct Register Manipulation  
> **Bench Tools & Debug:** Keil uVision MDK-ARM v5, ST-Link V2, Breadboard Hardware Bench  
> **Technical Documentation:** [View Architecture Report (Project_Report.pdf)](./Project_Report.pdf)

---

## Project Overview

This project demonstrates the implementation of Object-Oriented Programming (OOP) concepts in bare-metal embedded firmware on an **ARM Cortex-M3 microcontroller (STM32F103C8T6)**. 

By applying data encapsulation and abstraction directly over memory-mapped hardware registers, the design decouples peripheral control logic from application code without incurring runtime performance or memory overhead.

---

## Class Architecture & Hardware Model

```mermaid
classDiagram
    class LED {
        -LedColor_type color
        -LedState_type state
        +Led(LedColor_type _color)
        +Led_SetState(LedState_type _state)
    }
```

### Architectural Class Definitions & Design Steps

The driver models physical hardware components through a structured, object-oriented abstraction model without runtime memory or dynamic dispatch overhead:

- **Step 1: Hardware Identity & State Modeling**
  - **`LedColor_type`:** Strongly-typed enumeration mapping hardware channels (e.g., Port C pins PC13, PC14, PC15). Enforces compile-time type safety and prevents assigning invalid pin designations.
  - **`LedState_type`:** Strongly-typed enumeration representing logical actuation states (ON / OFF). Decouples application state logic from electrical active-low / active-high polarity.

- **Step 2: Data Encapsulation & Hardware Protection (Private Attributes)**
  - **`- color: LedColor_type`:** Private attribute storing the assigned hardware channel. Direct external mutation is blocked, protecting hardware configuration integrity.
  - **`- state: LedState_type`:** Private attribute tracking the current operational state of the peripheral instance.

- **Step 3: Deterministic Lifecycle & Actuation (Public Interface)**
  - **`+ Led(_color: LedColor_type)` (Constructor):** Handles automated hardware bring-up during object instantiation. Gates the peripheral clock on the APB2 bus (`RCC_APB2ENR`) and configures output direction and speed in the GPIO port configuration register (`GPIOC_CRH`), ensuring the hardware is valid prior to use.
  - **`+ Led_SetState(_state: LedState_type)` (Actuation Method):** Executes atomic bitmask writes directly to the port output data register (`GPIOC_ODR`), isolating neighboring pin states and avoiding read-modify-write hazards.

---

## Firmware Architecture & Low-Level Design

From a firmware engineering and low-level peripheral driver standpoint:

- **Direct Memory-Mapped Register Access:**
  - Configures peripheral bus clock gating via `RCC_APB2ENR` (`RCC_BASE + 0x18`, bit 4 `IOPCEN`) during class instantiation.
  - Controls GPIO configuration and pin mode through Port Configuration Register High (`GPIOC_CRH` at `0x40011004`), setting output mode to push-pull (2 MHz).
  - Directly drives and clears pin states via Port Output Data Register (`GPIOC_ODR` at `0x4001100C`).

- **Deterministic Constructor Initialization:**
  - The constructor executes peripheral clock enablement and port direction programming based on the passed color enum, guaranteeing hardware is ready before method calls occur.

- **Zero-Cost Abstraction Principles:**
  - Member functions operate directly on volatile register pointers without virtual table (vtable) dispatch, avoiding dynamic dispatch overhead and dynamic memory allocation (`malloc`/`new`).

---

## Hardware Bench Verification

- **Target Hardware:** STM32F103C8T6 Blue Pill board running at 72 MHz.
- **Actuators:** Multi-channel LEDs (PC13, PC14, PC15) wired in active-low sink configuration.
- **Validation:** Compiled via Keil MDK-ARM v5 and flashed via ST-Link V2. Verified deterministic state transitions, correct bitmask register operations, and clean scoping across independent class instances.

---

## Technical Documentation

- [Project_Report.pdf](./Project_Report.pdf): Engineering design report detailing register mapping, class diagrams, and bench results.
