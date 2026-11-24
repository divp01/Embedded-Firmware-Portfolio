# 8-Bit ALU Design & Hardware-in-the-Loop FPGA Verification

> **Core Technologies:** Verilog HDL, Altera (Intel) Cyclone II FPGA, Terasic DE2, Digital Logic Synthesis, Hardware-in-the-Loop (HIL)  
> **Bench Tools & Debug:** Dual Digilent Analog Discovery 2 (Pattern Generator + Logic Analyzer), Digilent WaveForms, Intel Quartus II  
> **Technical Documentation:** [View Architecture & Verification Report (Project_Report.pdf)](./Project_Report.pdf)

---

## Project Overview

This project designs, synthesizes, and physically validates an 8-bit Arithmetic and Logic Unit (ALU) using synthesizable Verilog HDL on an **Altera Cyclone II FPGA (EP2C35F672C6)** hosted on a Terasic DE2 development board.

To achieve robust verification, the design employs a **closed-loop Hardware-in-the-Loop (HIL)** test setup: one USB instrument acts as a digital stimulus pattern generator to drive operand inputs, while a second synchronized instrument operates as a high-speed logic analyzer to capture and verify real-time RTL output waveforms and conditional flags.

---

## ALU Architecture & Instruction Set

| Opcode (`[2:0]`) | Operation | Type | Function | Flag Updates |
|---|---|---|---|---|
| `000` | **ADD** | Arithmetic | `result = A + B` | Carry, Zero |
| `001` | **SUB** | Arithmetic | `result = A - B` | Carry, Zero |
| `010` | **MUL** | Arithmetic | `result = A * B` | Zero |
| `011` | **AND** | Logical | `result = A & B` | Zero |
| `100` | **OR** | Logical | `result = A | B` | Zero |
| `101` | **NAND** | Logical | `result = ~(A & B)` | Zero |
| `110` | **NOR** | Logical | `result = ~(A | B)` | Zero |
| `111` | **XOR** | Logical | `result = A ^ B` | Zero |

---

## Hardware Architecture & Low-Level Design

From a digital logic and hardware design standpoint:

- **Combinational Datapath & Register Width:**
  - Evaluates operands purely through combinational logic blocks (`always @*`), eliminating latch inference and minimizing propagation delay.
  - Features an expanded **16-bit output bus (`result[15:0]`)** to accommodate full precision product growth from 8x8-bit multiplication without truncation.

- **Status Flag Extraction:**
  - **Carry Flag:** Sampled directly from the 9th bit position (`result[8]`) on arithmetic additions and subtractions, providing immediate overflow/underflow status.
  - **Zero Flag:** Generated via hardware comparator logic (`result == 16'b0`) to support processor condition branching.

- **FPGA Pin Planning & Routing:**
  - Synthesized and mapped using Intel Quartus II.
  - Operands `A[7:0]`, `B[7:0]`, and opcode select lines routed across 40-pin GPIO expansion headers (`JP0`, `JP1`), interfacing directly to the external testbench harness.

---

## Hardware-in-the-Loop Bench Verification

The physical FPGA implementation was verified on a hardware bench using dual Digilent Analog Discovery 2 units:

- **Stimulus Injection (AD2 Unit 1):** Configured as a digital pattern generator to feed dynamic input vectors (operands A, B, and 3-bit opcodes) across the FPGA expansion header pins.
- **RTL Response Capture (AD2 Unit 2):** Configured as a high-speed logic analyzer running Digilent WaveForms, sampling the 16-bit result bus and conditional flags at 50 MHz.
- **Test Scenarios Verified:**
  - Negative overflow verification: `A = 0`, `B = 1`, `Opcode = 001 (SUB)` yielded `Result = 0x00FF` with `Carry = 1`.
  - Comprehensive truth table sweep: Tested arithmetic and bitwise logic operations with dynamic operands (`A = 20`, `B = 20`), confirming zero race conditions and 100% waveform agreement against RTL simulation.

---

## Technical Documentation

- [Project_Report.pdf](./Project_Report.pdf): Engineering design report detailing RTL schematics, pin planning, and hardware logic analyzer waveforms.
