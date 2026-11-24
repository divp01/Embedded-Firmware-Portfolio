#include "Led.h"

int main(void) {
    // Instantiate peripheral driver objects with deterministic hardware initialization
    Led led1(RED);
    Led led2(BLUE);
    Led led3(GREEN);

    // Actuate hardware channels
    led1.Led_SetState(ON);
    // led2.Led_SetState(ON);
    led3.Led_SetState(ON);

    // Embedded superloop
    while (1) {
    }

    return 0;
}
