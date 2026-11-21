/**
 * ==============================================================================
 * @file    main_systick.c
 * @brief   LESSON 1: Bare-Metal SysTick Timer Interrupt (ARM Cortex-M3 Core)
 * @author  Divyani Patel
 * ==============================================================================
 * 
 * CORE CONCEPT:
 * - SysTick is a 24-bit down-counter built INSIDE the ARM Cortex-M3 core (0xE000E010).
 * - Every 500 ms, the counter reaches 0 and hardware triggers Exception #15.
 * - Hardware automatically calls SysTick_Handler() to toggle PC13 LED.
 * - main() does zero work and sleeps in WFI (Wait For Interrupt) to save power.
 * ==============================================================================
 */

#include <stdint.h>

/* ==============================================================================
 * 1. HARDWARE REGISTER ADDRESSES
 * ============================================================================== */

/* Reset and Clock Control (RCC) */
#define RCC_BASE            (0x40021000UL)
#define RCC_APB2ENR         (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_APB2ENR_IOPCEN  (1U << 4)   /* Enable GPIO Port C Clock */

/* GPIO Port C */
#define GPIOC_BASE          (0x40011000UL)
#define GPIOC_CRH           (*(volatile uint32_t *)(GPIOC_BASE + 0x04)) /* Pins 8-15 */
#define GPIOC_ODR           (*(volatile uint32_t *)(GPIOC_BASE + 0x0C)) /* Output Data */
#define LED_PIN             (13U)       /* Blue Pill onboard LED: PC13 */

/* ARM Cortex-M3 SysTick Core Peripheral Registers (Address: 0xE000E010) */
#define SYST_CSR            (*(volatile uint32_t *)(0xE000E010UL)) /* Control & Status */
#define SYST_RVR            (*(volatile uint32_t *)(0xE000E014UL)) /* Reload Value */
#define SYST_CVR            (*(volatile uint32_t *)(0xE000E018UL)) /* Current Value */

#define SYST_CSR_ENABLE     (1U << 0)  /* Bit 0: Enable counter */
#define SYST_CSR_TICKINT    (1U << 1)  /* Bit 1: Enable Interrupt when counter reaches 0 */
#define SYST_CSR_CLKSOURCE  (1U << 2)  /* Bit 2: 1 = Core Clock (8 MHz HSI default) */

/* ==============================================================================
 * 2. INTERRUPT SERVICE ROUTINE (ISR)
 * Function name MUST match the symbol in startup.c
 * ============================================================================== */
void SysTick_Handler(void)
{
    /* Toggle PC13 LED (XOR operation) */
    GPIOC_ODR ^= (1U << LED_PIN);
}

/* ==============================================================================
 * 3. SYSTICK INITIALIZATION
 * ============================================================================== */
static void systick_init_ms(uint32_t ms)
{
    /*
     * CLOCK CALCULATION:
     * - Default clock = 8 MHz (8,000,000 cycles per second).
     * - In 1 ms: 8,000,000 / 1000 = 8,000 ticks.
     * - For 'ms' milliseconds: ticks = (8000 * ms) - 1.
     */
    uint32_t ticks = (8000U * ms) - 1U;

    /* 1. Stop SysTick while configuring */
    SYST_CSR = 0U;

    /* 2. Load Reload Value */
    SYST_RVR = ticks;

    /* 3. Clear Current Value */
    SYST_CVR = 0U;

    /* 4. Enable Counter, Enable Interrupt, Select 8 MHz Core Clock */
    SYST_CSR = SYST_CSR_CLKSOURCE | SYST_CSR_TICKINT | SYST_CSR_ENABLE;
}

/* ==============================================================================
 * 4. MAIN FUNCTION
 * ============================================================================== */
int main(void)
{
    /* Step 1: Enable clock for GPIO Port C */
    RCC_APB2ENR |= RCC_APB2ENR_IOPCEN;

    /* Step 2: Configure PC13 as Output Push-Pull (2 MHz) */
    uint32_t shift = (LED_PIN - 8U) * 4U; // (13 - 8) * 4 = 20
    GPIOC_CRH &= ~(0xFU << shift);        // Clear 4 bits
    GPIOC_CRH |=  (0x2U << shift);        // MODE=10 (2MHz), CNF=00 (Push-Pull)

    /* Turn off LED initially (PC13 is Active-Low: 1 = OFF) */
    GPIOC_ODR |= (1U << LED_PIN);

    /* Step 3: Start 500 ms SysTick Timer Interrupt */
    systick_init_ms(500);

    /* Step 4: Infinite Sleep Loop */
    while (1)
    {
        /*
         * CPU has nothing to do! 
         * WFI (Wait For Interrupt) puts CPU into sleep mode.
         * Every 500ms, SysTick hardware fires -> CPU wakes up ->
         * executes SysTick_Handler() -> returns to sleep!
         */
        __asm__ volatile ("wfi");
    }

    return 0;
}
