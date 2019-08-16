#include "common.h"

#include <util/delay.h>

void init_leds() {
    LED_DDR |= BIT(GENERAL_LED) | BIT(HALTING_LED);
}

void blinkn(uint8_t n, uint8_t led) {
    uint8_t i;
    for (i = 0; i < n * 2; i++) {
        LED_FLIP(led);
        _delay_ms(BLINK_DELAY);
    }
}
