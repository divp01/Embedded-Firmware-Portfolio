/**
 * ==============================================================================
 * @file    startup.c
 * @brief   Bare-Metal Startup Code for STM32F103 (Blue Pill) in pure C
 * @author  Divyani Patel
 * ==============================================================================
 */

#include <stdint.h>

/* Symbols from Linker Script */
extern uint32_t _estack;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

/* Forward declarations */
void Reset_Handler(void);
void Default_Handler(void);
extern int main(void);

/* Core ARM Exceptions */
void NMI_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void)  __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void)   __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void)     __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void)    __attribute__((weak, alias("Default_Handler")));

/* External Peripheral Interrupts (STM32F103) */
void WWDG_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void PVD_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void TAMPER_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void RTC_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void RCC_IRQHandler(void)                __attribute__((weak, alias("Default_Handler")));
void EXTI0_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void EXTI1_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void EXTI2_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void EXTI3_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void EXTI4_IRQHandler(void)              __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel2_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel3_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel4_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel5_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel6_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void DMA1_Channel7_IRQHandler(void)      __attribute__((weak, alias("Default_Handler")));
void ADC1_2_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void USB_HP_CAN1_TX_IRQHandler(void)     __attribute__((weak, alias("Default_Handler")));
void USB_LP_CAN1_RX0_IRQHandler(void)    __attribute__((weak, alias("Default_Handler")));
void CAN1_RX1_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void CAN1_SCE_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_IRQHandler(void)           __attribute__((weak, alias("Default_Handler")));
void TIM1_UP_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void TIM1_TRG_COM_IRQHandler(void)       __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void TIM2_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void TIM3_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void TIM4_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler(void)            __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler(void)               __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void USART3_IRQHandler(void)             __attribute__((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void RTC_Alarm_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));
void USBWakeUp_IRQHandler(void)          __attribute__((weak, alias("Default_Handler")));

/*
 * INTERRUPT VECTOR TABLE
 * Mapped to address 0x08000000 in Flash
 */
__attribute__((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    (void (*)(void))(&_estack),      /* 0x00: Initial Main Stack Pointer */
    Reset_Handler,                   /* 0x04: Reset Handler */
    NMI_Handler,                     /* 0x08: NMI */
    HardFault_Handler,               /* 0x0C: Hard Fault */
    MemManage_Handler,               /* 0x10: MemManage */
    BusFault_Handler,                /* 0x14: Bus Fault */
    UsageFault_Handler,              /* 0x18: Usage Fault */
    0, 0, 0, 0,                      /* 0x1C - 0x28: Reserved */
    SVC_Handler,                     /* 0x2C: SVCall */
    DebugMon_Handler,                /* 0x30: Debug Monitor */
    0,                               /* 0x34: Reserved */
    PendSV_Handler,                  /* 0x38: PendSV */
    SysTick_Handler,                 /* 0x3C: SysTick */

    /* External Interrupts (STM32F103) */
    WWDG_IRQHandler,                 /* 0x40: Window Watchdog */
    PVD_IRQHandler,                  /* 0x44: PVD through EXTI */
    TAMPER_IRQHandler,               /* 0x48: Tamper */
    RTC_IRQHandler,                  /* 0x4C: RTC */
    FLASH_IRQHandler,                /* 0x50: Flash */
    RCC_IRQHandler,                  /* 0x54: RCC */
    EXTI0_IRQHandler,                /* 0x58: EXTI Line 0 */
    EXTI1_IRQHandler,                /* 0x5C: EXTI Line 1 */
    EXTI2_IRQHandler,                /* 0x60: EXTI Line 2 */
    EXTI3_IRQHandler,                /* 0x64: EXTI Line 3 */
    EXTI4_IRQHandler,                /* 0x68: EXTI Line 4 */
    DMA1_Channel1_IRQHandler,        /* 0x6C: DMA1 Channel 1 */
    DMA1_Channel2_IRQHandler,        /* 0x70: DMA1 Channel 2 (SPI1_RX) */
    DMA1_Channel3_IRQHandler,        /* 0x74: DMA1 Channel 3 (SPI1_TX) */
    DMA1_Channel4_IRQHandler,        /* 0x78: DMA1 Channel 4 */
    DMA1_Channel5_IRQHandler,        /* 0x7C: DMA1 Channel 5 */
    DMA1_Channel6_IRQHandler,        /* 0x80: DMA1 Channel 6 */
    DMA1_Channel7_IRQHandler,        /* 0x84: DMA1 Channel 7 */
    ADC1_2_IRQHandler,               /* 0x88: ADC1 and ADC2 */
    USB_HP_CAN1_TX_IRQHandler,       /* 0x8C: USB High Priority / CAN TX */
    USB_LP_CAN1_RX0_IRQHandler,      /* 0x90: USB Low Priority / CAN RX0 */
    CAN1_RX1_IRQHandler,             /* 0x94: CAN RX1 */
    CAN1_SCE_IRQHandler,             /* 0x98: CAN SCE */
    EXTI9_5_IRQHandler,              /* 0x9C: EXTI Line 9..5 */
    TIM1_BRK_IRQHandler,             /* 0xA0: TIM1 Break */
    TIM1_UP_IRQHandler,              /* 0xA4: TIM1 Update */
    TIM1_TRG_COM_IRQHandler,         /* 0xA8: TIM1 Trigger and Commutation */
    TIM1_CC_IRQHandler,              /* 0xAC: TIM1 Capture Compare */
    TIM2_IRQHandler,                 /* 0xB0: TIM2 */
    TIM3_IRQHandler,                 /* 0xB4: TIM3 */
    TIM4_IRQHandler,                 /* 0xB8: TIM4 */
    I2C1_EV_IRQHandler,              /* 0xBC: I2C1 Event */
    I2C1_ER_IRQHandler,              /* 0xC0: I2C1 Error */
    I2C2_EV_IRQHandler,              /* 0xC4: I2C2 Event */
    I2C2_ER_IRQHandler,              /* 0xC8: I2C2 Error */
    SPI1_IRQHandler,                 /* 0xCC: SPI1 */
    SPI2_IRQHandler,                 /* 0xD0: SPI2 */
    USART1_IRQHandler,               /* 0xD4: USART1 */
    USART2_IRQHandler,               /* 0xD8: USART2 */
    USART3_IRQHandler,               /* 0xDC: USART3 */
    EXTI15_10_IRQHandler,            /* 0xE0: EXTI Line 15..10 */
    RTC_Alarm_IRQHandler,            /* 0xE4: RTC Alarm through EXTI */
    USBWakeUp_IRQHandler             /* 0xE8: USB Wakeup from suspend */
};

/*
 * RESET HANDLER
 */
void Reset_Handler(void)
{
    /* 1. Copy .data from Flash to SRAM */
    uint32_t *pSrc = &_sidata;
    uint32_t *pDst = &_sdata;

    while (pDst < &_edata) {
        *pDst++ = *pSrc++;
    }

    /* 2. Zero out .bss in SRAM */
    pDst = &_sbss;
    while (pDst < &_ebss) {
        *pDst++ = 0U;
    }

    /* 3. Jump to main() */
    main();

    /* Trap if main returns */
    while (1) {
        __asm__ volatile ("nop");
    }
}

void Default_Handler(void)
{
    while (1) {
        __asm__ volatile ("nop");
    }
}
