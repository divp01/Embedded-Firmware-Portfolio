#ifndef LED_H
#define LED_H

#include <stdint.h>

// Peripheral Base Addresses (STM32F103)
#define RCC_BASE      0x40021000UL
#define GPIOC_BASE    0x40011000UL

// Memory-Mapped Peripheral Registers
#define RCC_APB2ENR   (*(volatile uint32_t*)(RCC_BASE + 0x18))
#define GPIOC_CRH     (*(volatile uint32_t*)(GPIOC_BASE + 0x04))
#define GPIOC_ODR     (*(volatile uint32_t*)(GPIOC_BASE + 0x0C))

// Register Bitfield Definitions
#define RCC_IOPCEN    (1UL << 4)
#define GPIOC13       (1UL << 13)
#define GPIOC14       (1UL << 14)
#define GPIOC15       (1UL << 15)

// Strongly-Typed Enums
typedef enum {
    RED = 0,
    GREEN,
    BLUE
} LedColor_type;

typedef enum {
    OFF = 0,
    ON = 1
} LedState_type;

// C++ Hardware Abstraction Class
class Led {
private:
    LedColor_type color;
    LedState_type state;

public:
    Led(LedColor_type _color);
    void Led_SetState(LedState_type _state);
};

#endif // LED_H
