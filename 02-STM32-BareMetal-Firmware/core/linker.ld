/*
 * ==============================================================================
 * Linker Script: linker.ld
 * Target: STM32F103C8T6 (Blue Pill) - ARM Cortex-M3
 * Flash: 64 KB | SRAM: 20 KB
 * ==============================================================================
 */

ENTRY(Reset_Handler)

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K   /* 64 KB Internal Flash */
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K   /* 20 KB SRAM */
}

/* Calculate initial top of stack (End of 20K RAM) */
_estack = ORIGIN(RAM) + LENGTH(RAM); /* 0x20000000 + 0x5000 = 0x20005000 */

SECTIONS
{
    /* Code and Constants in Flash */
    .text :
    {
        . = ALIGN(4);
        KEEP(*(.isr_vector))    /* Interrupt Vector Table at offset 0x00 */
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;             /* End of code in Flash */
    } > FLASH

    /* Initialized Global/Static Variables (.data) */
    .data : AT (_etext)
    {
        . = ALIGN(4);
        _sdata = .;             /* Start of .data in RAM */
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;             /* End of .data in RAM */
    } > RAM

    _sidata = LOADADDR(.data);  /* Address in Flash where initial values live */

    /* Zero-initialized Variables (.bss) */
    .bss :
    {
        . = ALIGN(4);
        _sbss = .;              /* Start of .bss in RAM */
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;              /* End of .bss in RAM */
    } > RAM

    /DISCARD/ :
    {
        *(.ARM.exidx*)
        *(.ARM.extab*)
    }
}
