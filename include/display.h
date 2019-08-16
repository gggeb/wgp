// ST7735 DISPLAY

#ifndef DISPLAY
#define DISPLAY

#include "spi.h"

#include <util/delay.h>

#define DISPLAY_WIDTH  128
#define DISPLAY_HEIGHT 160

#define X_OFFSET      2
#define Y_OFFSET      1
#define WIDTH_OFFSET  2
#define HEIGHT_OFFSET 1

#define DISPLAY_DDR  DDRB
#define DISPLAY_PORT PORTB

enum DISPLAY_PINS {
    DC,
    RST,
    BL = 4
};

#define COLOUR_MODE 0x05 // 16bit colour

// 12 bit
// #define COLOUR(r, g, b) ((uint16_t)(((r >> 4) << 8) | ((g >> 4) << 4) | (b >> 4)))

// 16 bit
#define COLOUR(r, g, b) ((uint16_t)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)))

// -- COMMANDS -- //
// not a complete list
// refer to datasheet for more detail and extra commands, p78

#define DELAY_MASK 0x80

#define NOP     0x00 // no operation
#define SWRESET 0x01 // reset display in software
#define SLPIN   0x10 // enter sleep mode
#define SLPOUT  0x11 // exit sleep mode
#define PTLON   0x12 // turns partial mode on
#define NORON   0x13 // turn normal display on (turns partial mode off)
#define INVOFF  0x20 // disable colour inversion
#define INVON   0x21 // enable colour inversion
#define DISPOFF 0x28 // turn display off
#define DISPON  0x29 // turn display on
#define CASET   0x2A // set column boundaries, requires 4 parameters
#define RASET   0x2B // set row boundaries, requires 2 parameters (2 bytes each)
#define RAMWR   0x2C // write data to ram
#define RAMRD   0x2E // read from display memory
#define PTLAR   0x30 // set partial display area, requires 2 parameters (2 bytes each)
#define MADCTL  0x36 // memory data access control
#define COLMOD  0x3A // set colour mode, requires 1 parameter
#define FRMCTR1 0xB1 // sets frame rate for normal mode, requires 3 parameters
#define FRMCTR2 0xB2 // sets frame rate for idle mode
#define FRMCTR3 0xB3 // sets frame rate for partial mode
#define INVCTR  0xB4 // sets inversion mode, requires one parameter
#define GMCTRP1 0xE0 // sets positive polarity gamma correction, requires 16 parameters
#define GMCTRN1 0xE1 // sets negative polarity gamma correction, requires 16 parameters

// -- //

#define DELAY_MULTIPLE 10

void delay_hack(uint16_t ms); // 'ms' must be a multiple of DELAY_MULTIPLE

void write_data(uint8_t data);
void write_16bit(uint16_t data);
void write_cmd(uint8_t cmd);

void reset_display(void);

// requires 'commands' to be in PROG_MEM
void run_commands(uint8_t len, const uint8_t *commands);
void init_display(void);

void end_display(void);

void send_n565(uint16_t n, uint16_t data); // sends n packed colours to display

//
// y0 x0 -- x1
//     |    |
//     |    |
// y1 x0 -- x1
// 
void set_window(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1);

void fill_pixel(uint8_t x, uint8_t y, uint16_t colour);
void fill_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t colour);

void clear_with(uint16_t colour);

#endif
