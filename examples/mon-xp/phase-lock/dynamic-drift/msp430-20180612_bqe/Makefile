SRCS = blink.c
OBJS = $(SRCS:.c=.o)
TARGET = blink

#MCU=msp430f2013
MCU=msp430f2012

CFLAGS = -O0 -Wall -Werror -mmcu=$(MCU)

CC = msp430-gcc
OBJCOPY = msp430-objcopy

all: $(TARGET).hex

.SUFFIXES = .o .c .hex .bin

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET).bin: $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET).bin $(OBJS)

$(TARGET).hex: $(TARGET).bin
	$(OBJCOPY) -O ihex $(TARGET).bin $(TARGET).hex

upload: $(TARGET).hex
	mspdebug uif 'prog $(TARGET).hex'

.PHONY: clean
clean:
	rm -f $(TARGET).bin $(TARGET).hex $(OBJS)

