# wgp

this is a small project i worked on for a lil' moment to better understand
embedded systems and such. it uses the avr libc toolchain, and is programmed
for use with st7735 display and an arduino.

this would probably work with the raw atmega328 chip, but it work require some
rewiring work as you'd probably expect, and i feel like i'm gonna break the
atmega whenever i try to remove it from the arduino board, because i don't have
any proper microcontroller prying tools.

## pinout
### general
```
GENERAL LED -> 2
```
### input
```
UP    -> 4
RIGHT -> 5
DOWN  -> 6
LEFT  -> 7
```
### spi
```
SPI ACTION LED (HALTING LED) -> 3

MOSI -> 11
SCLK -> 13
```
### display
```
DC  -> 8
RST -> 9
BL  -> 12
```

all buttons and LEDs are active high i think - i'm no expert.

## issues

+ i wired my buttons up pretty poorly, so i personally have a bit of issue
turning the snake right, but that shouldn't be a problem if you're more
experienced than i.

+ sometimes the snake will randomly die on a turn, but i think this is because
i didn't debounce any inputs (the input component of this project is pretty
slapdash, i just threw it together 'cause the snake game is really just a test
of my display driver code).

i think that's all the bugs i remember, all the driver code and spi stuff is
pretty top-notch in my experience.

if you do find any issues feel free to tell me.
