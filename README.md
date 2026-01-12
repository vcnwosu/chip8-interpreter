# Custom CHIP-8 Emulator

I got bored one day and started thinking about emulators. I've never written one before, so I figure I'll try my hands at the CHIP-8. The instruction set is small and the overall architecture is seems simple enough. I doubt I will have time to dedicate to this in one single intensive "sprint", so I will just add to it bit by bit over time. For the most part, I am just playing around with the idea of an emulator.

## Project Structure

```
.
├── docs
│   ├── Chip8.pdf
│   └── Cowgod's Chip-8 Technical Reference.pdf
├── include
│   └── chip8.h
├── Makefile
├── README.md
├── src
│   ├── chip8.c
│   └── chip8c.c
└── test
    └── test.ch8

5 directories, 8 files
```

## Components

Two main components, the interpreter and assembler. The header file `./include/chip8.h` supports the implementation of both components.

### Assembler

This is a less-than-modest, custom CHIP-8 assembler. It supports __most__ of what seems to be the common/modern instructions. It's full-featured enough to assemble a binary for the `test/test.ch8` source file.

### Interpreter

This is again a less-than-modest, custom CHIP-8 interpreter. So far, it does support reading the binary and "loading it into memory", as well as the fetch/decode/execute cycle. The only thing is that the execute step is not __yet__ implemented. It's a stub for now.

## Executing

The `Makefile` does all the work. Simply run `make` (to compile the interpreter and assembler) or `make test` (to compile the assembler and use it to assemble the `test.ch8` sample source code, which can then be inspected with a simple `hexdump -C test/test`).


