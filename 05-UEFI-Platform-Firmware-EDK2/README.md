# Intel Server UEFI Platform Firmware Development (EDK II)

> **Core Technologies:** C, x86_64 Architecture, Intel Server Platforms, UEFI Platform Initialization (PI), EDK II (TianoCore)  
> **Development Tools:** Visual Studio, EDK2 Build Environment, Git, Tianocore Tools  
> **Technical Documentation:** [View Project Presentation Deck (Project_Presentation.pptx)](./Project_Presentation.pptx)

---

## Project Overview

This project focuses on enterprise platform firmware development for Intel Server architectures using the Unified Extensible Firmware Interface (UEFI) Platform Initialization (PI) framework built on TianoCore EDK II.

Platform initialization establishes low-level hardware readiness from initial power-on reset (POR) through Cache-as-RAM (CAR) setup, memory controller training, chipset bus enumeration, and handoff to the operating system bootloader.

---

## UEFI Boot Phases & System Flow

The platform firmware executes across structured initialization stages:

1. **Security (SEC) Phase:**
   - First code executed upon CPU reset out of reset vector.
   - Transitions the CPU from 16-bit real mode into protected mode.
   - Configures CPU L1/L2 caches as temporary stack memory (Cache-as-RAM) before physical system DRAM is initialized.

2. **Pre-EFI Initialization (PEI) Phase:**
   - Discovers and initializes system memory controllers, Southbridge I/O channels, and point-to-point interconnects.
   - Executes Pre-EFI Initialization Modules (PEIMs) to configure memory and construct Handoff Blocks (HOBs).

3. **Driver Execution Environment (DXE) Phase:**
   - Unpacks and executes core hardware drivers, PCI/PCIe bus enumeration, and UEFI architectural protocols once full system DRAM is accessible.

4. **Boot Device Selection (BDS) Phase:**
   - Enforces Secure Boot policies, selects target boot storage according to platform priority, and loads the OS bootloader.

---

## Firmware Architecture & Low-Level Design

From a platform firmware engineering standpoint:

- **Modular PEIM Communication via PPI:**
  - Implemented inter-module communication in the PEI phase using the PEIM-to-PEIM Interface (PPI) architectural pattern.
  - **Installer PEIM:** Allocates and populates structured platform status and hardware configuration data, publishing a unique GUID-identified interface descriptor into the PEI Core database.
  - **Locator PEIM:** Discovers the published PPI via GUID lookup, resolving the function pointer table to safely exchange telemetry without tight coupling or hardcoded memory addresses.

- **EDK II Build Hierarchy & Package Structure:**
  - Authored standard EDK2 configuration files: Module Information (`.inf`), Package Declaration (`.dec`), and Platform Description (`.dsc`).
  - Managed module entry points, library classes (`BaseLib`, `PeimEntryPoint`), and GUID declarations compliant with TianoCore coding standards.

- **Memory Constraints in Pre-Memory Environments:**
  - Designed PEI routines under strict Cache-as-RAM space limits, avoiding heap allocations and relying on early stack descriptors until DRAM discovery completes.

---

## Technical Documentation

- [Project_Presentation.pptx](./Project_Presentation.pptx): Architecture review slide deck detailing platform boot phases, EDK II build flow, and PEIM module implementation.
