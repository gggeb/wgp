CC       = avr-gcc
OBJCOPY  = avr-objcopy
OBJDUMP  = avr-objdump
UPLOADER = avrdude

MCU_TARGET = atmega328p

PROGRAMMER_DEV  = /dev/ttyACM3
PROGRAMMER_TYPE = arduino

NAME = wgp
OBJS = common.o spi.o display.o snake.o

SOURCE  = ./src
INCLUDE = ./include

CFLAGS = -Wall -I $(INCLUDE) -mmcu=$(MCU_TARGET) -Os

.PHONY: info build upload clean

info:
	@echo "\t'build' : compiles $(NAME)"
	@echo "\t'upload': uses $(UPLOADER) to upload $(NAME) to the $(MCU_TARGET)"
	@echo "\t'clean' : deletes generated files"

%.o: $(SOURCE)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(NAME).elf: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(NAME).hex: $(NAME).elf
	avr-objcopy -j .text -j .data -O ihex $^ $@

build: $(NAME).hex

upload: $(NAME).hex
	$(UPLOADER) -p $(MCU_TARGET) -P $(PROGRAMMER_DEV) -c $(PROGRAMMER_TYPE) \
		-v -U flash:w:$^

clean:
	rm $(OBJS) $(NAME).elf $(NAME).hex
