#include "display.h"

#include <avr/pgmspace.h>

// commands ran during initialization
// format:
//
// CMD, TOTAL ARGUMENTS [| DELAY MASK],
//     ARGUMENT_1,
//     ...
//     ARGUMENT_N,
//     [DELAY LENGTH]
//

static const uint8_t init_commands_len = 7;
static const uint8_t init_commands[] PROGMEM = {
    SLPOUT, DELAY_MASK,
        250,

    COLMOD, 1,
        COLOUR_MODE,

    FRMCTR1, 3,
        0x00, // fastest refresh rate
        0x06, // 6 line front porch
        0x03, // 3 line back porch

    GMCTRP1, 16,                // gamma correction magic
        0x09, 0x16, 0x09, 0x20, //
        0x21, 0x1B, 0x13, 0x19, //
        0x17, 0x15, 0x1E, 0x2B, //
        0x04, 0x05, 0x02, 0x0E, //
                                //
    GMCTRN1, 16,                //
        0x0B, 0x14, 0x08, 0x1E, //
        0x22, 0x1D, 0x18, 0x1E, //
        0x1B, 0x1A, 0x24, 0x2B, //
        0x06, 0x06, 0x02, 0x0F, //

    NORON,  0,
    DISPON, 0,
};

void delay_hack(uint16_t ms) {
    uint8_t i;
    for (i = 0; i < ms; i += DELAY_MULTIPLE)
        _delay_ms(DELAY_MULTIPLE);
}

void write_data(uint8_t data) {
    BIT_ON(DISPLAY_PORT, DC);
    tranceive_spi(data);
}

void write_16bit(uint16_t data) {
    write_data(data >> 8);
    write_data(data & 0xFF);
}

void write_cmd(uint8_t cmd) {
    BIT_OFF(DISPLAY_PORT, DC);
    tranceive_spi(cmd);
}

void reset_display(void) {
    BIT_OFF(DISPLAY_PORT, RST);
    _delay_ms(1);
    BIT_ON(DISPLAY_PORT, RST);
    _delay_ms(200);
}

void run_commands(uint8_t len, const uint8_t *commands) {
    uint8_t i, args, delay;
    i = len;

    while (i--) {
        write_cmd(pgm_read_byte(commands++));
        
        delay = args = pgm_read_byte(commands++);

        args &= ~DELAY_MASK;
        delay &= DELAY_MASK;

        while (args--)
            write_data(pgm_read_byte(commands++));

        if (delay)
            delay_hack(pgm_read_byte(commands++));
    }
}

void init_display(void) {
    DISPLAY_DDR |= BIT(BL) | BIT(RST) | BIT(DC);
    BIT_ON(DISPLAY_PORT, RST);

    reset_display();

    run_commands(init_commands_len, &init_commands[0]);
}

void end_display(void) {
    reset_display();
    BIT_OFF(DISPLAY_PORT, BL);
    
    write_cmd(SLPIN);
    _delay_ms(150);

    write_cmd(DISPOFF);
}

void send_n565(uint16_t n, uint16_t data) {
    uint16_t i;

    write_cmd(RAMWR);

    for (i = 0; i < n; i++)
        write_16bit(data);
}

void set_window(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1) {
    write_cmd(CASET);

    write_data(0);
    write_data(x0 + X_OFFSET);
    write_data(0);
    write_data(x1 + WIDTH_OFFSET);

    write_cmd(RASET);

    write_data(0);
    write_data(y0 + Y_OFFSET);
    write_data(0);
    write_data(y1 + HEIGHT_OFFSET);
}

void fill_pixel(uint8_t x, uint8_t y, uint16_t colour) {
    if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT)
        return;

    set_window(x, x, y, y);
    send_n565(1, colour);
}

void fill_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t colour) {
    set_window(x, x + width - 1, y, y + height - 1);
    send_n565(width * height, colour);
}

void clear_with(uint16_t colour) {
    set_window(0, DISPLAY_WIDTH - 1, 0, DISPLAY_HEIGHT - 1);
    send_n565(DISPLAY_WIDTH * DISPLAY_HEIGHT, colour);
}
