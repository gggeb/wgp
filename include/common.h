#ifndef COMMON
#define COMMON

#define F_CPU 16000000UL // atmega328p's clock speed 20MHz

#include <stdint.h>
#include <avr/io.h>

// -- IO MACROS -- //

#define BIT(n) (1 << n)

#define GET_BIT(p, n) (p & BIT(n))

#define BIT_OFF(p, n)  (p &= ~BIT(n))
#define BIT_ON(p, n)   (p |= BIT(n))
#define BIT_FLIP(p, n) (p ^= BIT(n))

// -- STATUS LEDS -- //

#define LED_DDR  DDRD
#define LED_PORT PORTD

#define GENERAL_LED 2
#define HALTING_LED 3

#define LED_OFF(l)  BIT_OFF(LED_PORT, l)
#define LED_ON(l)   BIT_ON(LED_PORT, l)
#define LED_FLIP(l) BIT_FLIP(LED_PORT, l)

#define BLINK_DELAY 250 // 2Hz

void init_leds(void);                // sets DDR for LEDs
void blinkn(uint8_t n, uint8_t led); // blinks 'led' n times

#endif
