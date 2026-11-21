# STM32F103 (Blue Pill) Bare-Metal Firmware Labs

**Target MCU:** STM32F103C8T6 (ARM Cortex-M3 @ 8-72 MHz)  
**Toolchain:** `arm-none-eabi-gcc` (Pure bare-metal: No HAL, No CMSIS, `-nostdlib`)  
**Design Pattern:** Modular driver architecture, register-level hardware programming, low-power interrupts.

---

## 📁 Repository Directory Structure

```
BluePill_Blink/
├── app/                              # Standalone lesson applications
│   ├── 01_systick_blink.c            # Lesson 1: Pure SysTick Timer Interrupt & WFI Sleep
│   ├── 02_spi_ringbuffer.c           # Lesson 2: Continuous Streaming via SPI Interrupt & FIFO
│   └── 03_spi_dma_pingpong.c         # Lesson 3: High-Speed Continuous SPI via Circular DMA
├── core/                             # Architecture startup & memory layout
│   ├── linker.ld                     # Flash (64KB @ 0x08000000) & SRAM (20KB @ 0x20000000)
│   └── startup.c                    # Vector table (ARM exceptions + peripheral IRQs) & Reset Handler
├── drivers/                          # Reusable hardware peripheral drivers
│   ├── inc/
│   │   ├── spi.h                     # Bare-metal SPI1 driver header (Master, Polling, IRQ, DMA)
│   │   └── ring_buffer.h             # Lock-free circular FIFO header (Power-of-2 bitwise masking)
│   └── src/
│       ├── spi.c                     # Bare-metal SPI1 driver implementation
│       └── ring_buffer.c             # Lock-free circular FIFO implementation
├── out/                              # Generated binaries (.elf, .bin, .map, .o)
├── Makefile                          # Unified build automation & flash commands
└── README.md                         # Documentation & laboratory guide
```

---

## 🎯 The 3 Educational Lessons

### Lesson 1: [01_systick_blink.c](file:///d:/Development/Firmware/BluePill_Blink/app/01_systick_blink.c)
- **Concept:** ARM Cortex-M3 core timer exception (Exception #15).
- **Behavior:** Configures the internal 24-bit SysTick down-counter (`0xE000E010`) to reload every 500 ms.
- **Power Savings:** The `main()` thread sleeps in `wfi` (Wait For Interrupt, 0% CPU consumption). The hardware timer wakes the CPU every 500 ms to execute `SysTick_Handler()` and toggle PC13 (Active-Low onboard LED).

### Lesson 2: [02_spi_ringbuffer.c](file:///d:/Development/Firmware/BluePill_Blink/app/02_spi_ringbuffer.c)
- **Concept:** Producer-Consumer pattern for continuous streaming data.
- **Behavior:**
  - **Producer (`SPI1_IRQHandler`):** Captures incoming bytes from `SPI1_DR` in under 1 microsecond and pushes them into a 128-byte circular FIFO using bitwise wrapping:
    $$\text{next\_tail} = (\text{tail} + 1) \ \& \ 127$$
  - **Consumer (`main()`):** Pops bytes from the FIFO at its own pace and parses them without dropping data or triggering Overrun (`OVR`) errors.

### Lesson 3: [03_spi_dma_pingpong.c](file:///d:/Development/Firmware/BluePill_Blink/app/03_spi_dma_pingpong.c)
- **Concept:** Zero-copy high-throughput streaming (e.g. 10 MHz SPI ADC/Audio).
- **Behavior:**
  - Configures **DMA1 Channel 2** in hardware `CIRCULAR` mode with a 256-byte double buffer (128 bytes Ping + 128 bytes Pong).
  - Bytes move from SPI to SRAM completely in hardware with **0% CPU cycle overhead**.
  - **Half-Transfer (`HT`) Interrupt:** Fires when Ping is full -> CPU processes Ping while DMA fills Pong.
  - **Transfer-Complete (`TC`) Interrupt:** Fires when Pong is full -> CPU processes Pong while DMA refills Ping.

---

## ⚡ How to Build

Run `make` to compile all 3 standalone lessons in one go:
```bash
make
```

Or build a specific lesson:
```bash
make lesson1    # Builds out/01_systick_blink.bin
make lesson2    # Builds out/02_spi_ringbuffer.bin
make lesson3    # Builds out/03_spi_dma_pingpong.bin
```

Clean build artifacts:
```bash
make clean
```

---

## 🔌 Hardware Pinout on Blue Pill (STM32F103)

### Onboard LED:
- **PC13:** Active-Low User LED (Drive LOW = ON, Drive HIGH = OFF).

### SPI1 Master (Port A):
| Pin | Function | Configuration in `GPIOA_CRL` | Notes |
|---|---|---|---|
| **PA4** | **CS / NSS** | Output Push-Pull 10 MHz (`0x1`) | Software GPIO controlled (`spi1_cs_enable / disable`) |
| **PA5** | **SCK** | Alternate Function Push-Pull 10 MHz (`0x9`) | 1 MHz SPI Clock |
| **PA6** | **MISO** | Input Pull-Up (`0x8`) | Data In from external slave |
| **PA7** | **MOSI** | Alternate Function Push-Pull 10 MHz (`0x9`) | Data Out to external slave |

---

## 🚀 How to Flash (ST-Link V2)

### Using Makefile Shortcuts (via OpenOCD):
```bash
make flash1     # Flashes Lesson 1
make flash2     # Flashes Lesson 2
make flash3     # Flashes Lesson 3
```

### Manual OpenOCD Command:
```bash
openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program out/01_systick_blink.bin 0x08000000 verify reset exit"
```

### Using STM32CubeProgrammer CLI:
```bash
STM32_Programmer_CLI -c port=SWD -w out/01_systick_blink.bin 0x08000000 -v -rst
```
