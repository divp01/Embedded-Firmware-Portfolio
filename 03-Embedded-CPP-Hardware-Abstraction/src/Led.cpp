#include "Led.h"

Led::Led(LedColor_type _color) {
    this->color = _color;

    // Default pins high (active-low sink configuration)
    GPIOC_ODR |= 0xFFFFFFFF;

    // Enable Port C peripheral clock on APB2 bus
    RCC_APB2ENR |= RCC_IOPCEN;

    // Configure GPIO Mode: Output push-pull, max speed 2 MHz
    switch (this->color) {
        case RED:
            GPIOC_CRH &= 0xFF0FFFFFUL;
            GPIOC_CRH |= 0x00200000UL;
            break;

        case BLUE:
            GPIOC_CRH &= 0xF0FFFFFFUL;
            GPIOC_CRH |= 0x02000000UL;
            break;

        case GREEN:
            GPIOC_CRH &= 0x0FFFFFFFUL;
            GPIOC_CRH |= 0x20000000UL;
            break;
    }
}

void Led::Led_SetState(LedState_type _state) {
    this->state = _state;

    // Direct atomic bitmask register write (active-low logic)
    switch (this->color) {
        case RED:
            if (this->state == OFF) {
                GPIOC_ODR |= GPIOC13;
            } else {
                GPIOC_ODR &= (~GPIOC13);
            }
            break;

        case BLUE:
            if (this->state == OFF) {
                GPIOC_ODR |= GPIOC14;
            } else {
                GPIOC_ODR &= (~GPIOC14);
            }
            break;

        case GREEN:
            if (this->state == OFF) {
                GPIOC_ODR |= GPIOC15;
            } else {
                GPIOC_ODR &= (~GPIOC15);
            }
            break;
    }
}
