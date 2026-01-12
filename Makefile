CC = gcc
CFLAGS = -Wall -g -I./include

BUILD_DIR = build

CHIP8_ASM = $(BUILD_DIR)/chip8c
CHIP8_INT = $(BUILD_DIR)/chip8

all: $(CHIP8_INT) $(CHIP8_ASM)

$(BUILD_DIR):
	mkdir -p $@

$(CHIP8_INT): build src/chip8.c
	$(CC) $(CFLAGS) -o $@ src/chip8.c

$(CHIP8_ASM): build src/chip8c.c
	$(CC) $(CFLAGS) -o $@ src/chip8c.c

test: $(CHIP8_ASM)
	$(CHIP8_ASM) test/test.ch8
	# TODO: Implement some actual tests

clean:
	rm -rf build test/test

.PHONY: all clean test
