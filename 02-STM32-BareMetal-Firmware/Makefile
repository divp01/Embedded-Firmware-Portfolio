# ==============================================================================
# Makefile: STM32F103 (Blue Pill) Bare-Metal Firmware Labs
# ==============================================================================
# Directory Structure:
#   ├── core/           Startup vector table & linker script
#   ├── drivers/        Bare-metal peripheral drivers (SPI, Ring Buffer)
#   │   ├── inc/        Header files (.h)
#   │   └── src/        Source files (.c)
#   ├── app/            Individual lesson applications (.c)
#   └── out/            Generated binaries (.elf, .bin, .map, .o)
# ==============================================================================

# Toolchain
CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

# Project Directories
CORE_DIR = core
DRV_DIR  = drivers
APP_DIR  = app
OUT_DIR  = out

# Linker Script
LDSCRIPT = $(CORE_DIR)/linker.ld

# Compiler & Linker Flags
MCU_FLAGS = -mcpu=cortex-m3 -mthumb
CFLAGS    = $(MCU_FLAGS) -Wall -Wextra -Werror -O2 -std=c99 -nostdlib -ffreestanding \
            -I$(DRV_DIR)/inc -I$(CORE_DIR)
LDFLAGS   = $(MCU_FLAGS) -T $(LDSCRIPT) -nostdlib

# Common Core & Driver Objects
STARTUP_OBJ    = $(OUT_DIR)/startup.o
DRIVERS_OBJS   = $(OUT_DIR)/spi.o $(OUT_DIR)/ring_buffer.o
COMMON_OBJS    = $(STARTUP_OBJ) $(DRIVERS_OBJS)

# All Application Targets
TARGETS = 01_systick_blink 02_spi_ringbuffer 03_spi_dma_pingpong

# Default: build all lessons
all: $(OUT_DIR) $(TARGETS)

# Create build directory
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# --- Compilation Rules ---
$(OUT_DIR)/startup.o: $(CORE_DIR)/startup.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/spi.o: $(DRV_DIR)/src/spi.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/ring_buffer.o: $(DRV_DIR)/src/ring_buffer.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/%.o: $(APP_DIR)/%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Target 1: Lesson 1 - SysTick Timer Interrupt & LED Blink ---
$(OUT_DIR)/01_systick_blink.elf: $(STARTUP_OBJ) $(OUT_DIR)/01_systick_blink.o $(LDSCRIPT)
	$(CC) $(STARTUP_OBJ) $(OUT_DIR)/01_systick_blink.o $(LDFLAGS) -Wl,-Map=$(OUT_DIR)/01_systick_blink.map,--cref -o $@

$(OUT_DIR)/01_systick_blink.bin: $(OUT_DIR)/01_systick_blink.elf
	$(OBJCOPY) -O binary $< $@

01_systick_blink: $(OUT_DIR)/01_systick_blink.bin
	@echo ---------------- [Lesson 1: SysTick Blink] Memory Footprint ----------------
	@$(SIZE) $(OUT_DIR)/01_systick_blink.elf
	@echo "Build successful: $(OUT_DIR)/01_systick_blink.bin"
	@echo ----------------------------------------------------------------------------

# --- Target 2: Lesson 2 - Continuous SPI via Interrupt + Ring Buffer ---
$(OUT_DIR)/02_spi_ringbuffer.elf: $(COMMON_OBJS) $(OUT_DIR)/02_spi_ringbuffer.o $(LDSCRIPT)
	$(CC) $(COMMON_OBJS) $(OUT_DIR)/02_spi_ringbuffer.o $(LDFLAGS) -Wl,-Map=$(OUT_DIR)/02_spi_ringbuffer.map,--cref -o $@

$(OUT_DIR)/02_spi_ringbuffer.bin: $(OUT_DIR)/02_spi_ringbuffer.elf
	$(OBJCOPY) -O binary $< $@

02_spi_ringbuffer: $(OUT_DIR)/02_spi_ringbuffer.bin
	@echo ------------ [Lesson 2: SPI Ring Buffer] Memory Footprint -------------
	@$(SIZE) $(OUT_DIR)/02_spi_ringbuffer.elf
	@echo "Build successful: $(OUT_DIR)/02_spi_ringbuffer.bin"
	@echo ----------------------------------------------------------------------------

# --- Target 3: Lesson 3 - High-Speed Continuous SPI via Circular DMA Ping-Pong ---
$(OUT_DIR)/03_spi_dma_pingpong.elf: $(COMMON_OBJS) $(OUT_DIR)/03_spi_dma_pingpong.o $(LDSCRIPT)
	$(CC) $(COMMON_OBJS) $(OUT_DIR)/03_spi_dma_pingpong.o $(LDFLAGS) -Wl,-Map=$(OUT_DIR)/03_spi_dma_pingpong.map,--cref -o $@

$(OUT_DIR)/03_spi_dma_pingpong.bin: $(OUT_DIR)/03_spi_dma_pingpong.elf
	$(OBJCOPY) -O binary $< $@

03_spi_dma_pingpong: $(OUT_DIR)/03_spi_dma_pingpong.bin
	@echo ----------- [Lesson 3: SPI DMA Ping-Pong] Memory Footprint -----------
	@$(SIZE) $(OUT_DIR)/03_spi_dma_pingpong.elf
	@echo "Build successful: $(OUT_DIR)/03_spi_dma_pingpong.bin"
	@echo ----------------------------------------------------------------------------

# Aliases for convenience
lesson1: 01_systick_blink
lesson2: 02_spi_ringbuffer
lesson3: 03_spi_dma_pingpong

# --- Flashing Shortcuts (OpenOCD + ST-Link) ---
flash1: $(OUT_DIR)/01_systick_blink.bin
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(OUT_DIR)/01_systick_blink.bin 0x08000000 verify reset exit"

flash2: $(OUT_DIR)/02_spi_ringbuffer.bin
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(OUT_DIR)/02_spi_ringbuffer.bin 0x08000000 verify reset exit"

flash3: $(OUT_DIR)/03_spi_dma_pingpong.bin
	openocd -f interface/stlink.cfg -f target/stm32f1x.cfg -c "program $(OUT_DIR)/03_spi_dma_pingpong.bin 0x08000000 verify reset exit"

clean:
	rm -rf $(OUT_DIR)

.PHONY: all clean $(TARGETS) lesson1 lesson2 lesson3 flash1 flash2 flash3
