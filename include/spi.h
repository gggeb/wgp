#ifndef SPI
#define SPI

#include "common.h"

#define SPI_DDR  DDRB
#define SPI_PORT PORTB

enum SPI_PINS {
    SS = 2,
    MOSI,
    MISO,
    SCLK
};

void init_spi(void);
void end_spi(void);

uint8_t tranceive_spi(uint8_t data);

#endif
