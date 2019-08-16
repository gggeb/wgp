// currently only directional input

#include "common.h"

#define INPUT_DDR  DDRD
#define INPUT_PORT PIND

enum DPAD_PINS {
    UP = 4,
    RIGHT,
    DOWN,
    LEFT
};

#define GET_BUTTON(b) GET_BIT(INPUT_PORT, b)
