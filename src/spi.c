#include "spi.h"

void init_spi(void) {
    SPI_DDR |= BIT(SS) | BIT(MOSI) | BIT(SCLK);

    SPCR |= BIT(SPE) | BIT(MSTR);
    BIT_ON(SPSR, SPI2X);

    // apparently the display flickers if MOSI is low between transmissions
    BIT_ON(SPI_PORT, MOSI);
}

void end_spi(void) {
    SPCR = 0;
}

uint8_t tranceive_spi(uint8_t data) {
    SPDR = data;

    LED_ON(HALTING_LED);
    while (!GET_BIT(SPSR, SPIF))
        ;
    LED_OFF(HALTING_LED);
    
    return SPDR;
}
