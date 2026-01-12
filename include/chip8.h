/************************************
 * chip8.h - header file for CHIP-8 interpreter
 *
 * Developer: Victor Nwosu
 ***********************************/

#ifndef _CHIP8_H
#define _CHIP8_H

/**
 * includes
 */
#include <stdint.h>
#include <stdlib.h>

/**
 * general-purpose registers
 *
 * CHIP-8 specification requires the use of 16
 * general-purpose 8-bit registers (V0 - VF)
 */
#define CHIP8_GP_REGS 16
#define CHIP8_V0 0
#define CHIP8_V1 1
#define CHIP8_V2 2
#define CHIP8_V3 3
#define CHIP8_V4 4
#define CHIP8_V5 5
#define CHIP8_V6 6
#define CHIP8_V7 7
#define CHIP8_V9 9
#define CHIP8_VA 10
#define CHIP8_VB 11
#define CHIP8_VC 12
#define CHIP8_VD 13
#define CHIP8_VE 14
#define CHIP8_VF 15

/**
 * special-purpose registers
 *
 * CHIP-8 specification supports the following
 * special-purpose registers.
 *
 * 16-bit program counter
 * 16-bit index register
 * 8-bit stack pointer register
 * 8-bit delay register
 * 8-bit sound timer register 
 */
#define CHIP8_SP_REGS 5
#define CHIP8_PC 0
#define CHIP8_IX 1
#define CHIP8_SP 2
#define CHIP8_DL 3
#define CHIP8_ST 4

/**
 * memory capacty
 *
 * CHIP-8 specification supports 4096 bytes
 * of addressable memory
 */
#define CHIP8_MEMORY_CAPACITY 4096

/**
 * the stack
 *
 * CHIP-8 specification supports a stack size
 * of 16 16-bit values, used to store the address
 * that the interpreter should return to when
 * finished with a subroutine
 */
#define CHIP8_STACK_SIZE 16

/**
 * rs1: register set 1
 * the 16 general purpose registers
 */
uint8_t rs1[CHIP8_GP_REGS];

/**
 * rs2: register set 2
 * the program counter, index register
 * stack pointer, delay timer and sound timer
 */
uint16_t rs2[CHIP8_SP_REGS];

/**
 * 4096 main memory
 */
uint8_t memory[CHIP8_MEMORY_CAPACITY];

/**
 * the 256-bit stack
 */
uint16_t stack[CHIP8_STACK_SIZE];

/**
 * the operation code table
 */
typedef enum {
    CHIP8_OPR_IX, // CHIP-8 instruction
    CHIP8_OPR_DR, // ASM directive
} chip8_operator;

typedef enum {
    CHIP8_WIDTH_BYTE,
    CHIP8_WIDTH_WORD,
} chip8_translation_width;

typedef enum {
    CHIP8_OP_NONE,
    CHIP8_OP_REG8,
    CHIP8_OP_REG4,
    CHIP8_OP_REG0,
    CHIP8_OP_DT, // delay time register
    CHIP8_OP_ST, // sound timer register
    CHIP8_OP_IX, // index register
    CHIP8_OP_IXR, // index register referenced
    CHIP8_OP_BCD, // store BCD representation
    CHIP8_OP_SPRITE, // hexadecimal sprites
    CHIP8_OP_HF, // 10-byte hexadecimal font sprite
    CHIP8_OP_NIBBLE, // 4-bit
    CHIP8_OP_BYTE, // 8-bit
    CHIP8_OP_SLAB, // 12-bit
    CHIP8_OP_NULL,
    CHIP8_OP_KEY, // keypress
} chip8_operands;

#define CHIP8_OP_CLS            0x00E0
#define CHIP8_OP_RET            0x00EE
#define CHIP8_OP_SCR            0x00FB
#define CHIP8_OP_SCL            0x00FC
#define CHIP8_OP_EXIT           0x00FD
#define CHIP8_OP_LOW            0x00FE
#define CHIP8_OP_HIGH           0x00FF
#define CHIP8_OP_SCD            0x00C0
#define CHIP8_OP_JP             0x1000
#define CHIP8_OP_CALL           0x2000
#define CHIP8_OP_LD_ADDR        0xA000
#define CHIP8_OP_JP_V0          0xB000
#define CHIP8_OP_SE_BYTE        0x3000
#define CHIP8_OP_SNE_BYTE       0x4000
#define CHIP8_OP_LD_BYTE        0x6000
#define CHIP8_OP_ADD_BYTE       0x7000
#define CHIP8_OP_RND_BYTE       0xC000
#define CHIP8_OP_SE_REG         0x5000
#define CHIP8_OP_LD_REG         0x8000
#define CHIP8_OP_OR             0x8001
#define CHIP8_OP_AND            0x8002
#define CHIP8_OP_XOR            0x8003
#define CHIP8_OP_ADD_REG        0x8004
#define CHIP8_OP_SUB            0x8005
#define CHIP8_OP_SHR            0x8006
#define CHIP8_OP_SUBN           0x8007
#define CHIP8_OP_SHL            0x800E
#define CHIP8_OP_SNE_REG        0x9000
#define CHIP8_OP_SKP            0xE09E
#define CHIP8_OP_SKNP           0xE0A1
#define CHIP8_OP_LD_DT          0xF007
#define CHIP8_OP_LD_KEY         0xF00A
#define CHIP8_OP_LD_REG_DT      0xF015
#define CHIP8_OP_LD_REG_ST      0xF018
#define CHIP8_OP_ADD_REG_IX     0xF01E
#define CHIP8_OP_LD_SPRITE      0xF029
#define CHIP8_OP_LDS_BCD        0xF033
#define CHIP8_OP_LDS_REGS       0xF055
#define CHIP8_OP_LD_REGS        0xF065
#define CHIP8_OP_DRW_EXT        0xD000
#define CHIP8_OP_DRW_NIBBLE     0xD000
#define CHIP8_OP_DB             0x0000




#define CHIP8_OP_MASK_LSN 0x000F
#define CHIP8_OP_MASK_LSB 0x00FF
#define CHIP8_OP_MASK_LSS 0x0FFF
#define CHIP8_OP_MASK_WRD 0xFFFF
#define CHIP8_OP_MASK_GSN 0xF000
#define CHIP8_OP_MASK_GSB 0xFF00
#define CHIP8_OP_MASK_GSS 0xFFF0
#define CHIP8_OP_MASK_EXT 0xF00F
#define CHIP8_OP_MASK_EXX 0xF0FF
#define CHIP8_OP_MASK_VX8 0x0F00
#define CHIP8_OP_MASK_VX4 0x00F0

typedef struct {
    char *mnemonic;
    uint16_t opcode;
    uint16_t mask;
    chip8_operator opr;
    chip8_operands operands[3];
} chip8_operations;

chip8_operations optab[] = {
    { // CLS -> 00E0
        "CLS",
        CHIP8_OP_CLS,
        CHIP8_OP_MASK_WRD,
        CHIP8_OPR_IX,
        { CHIP8_OP_NONE, CHIP8_OP_NONE, CHIP8_OP_NONE }
    },
    { // RET -> 00EE
        "RET",
        CHIP8_OP_RET,
        CHIP8_OP_MASK_WRD,
        CHIP8_OPR_IX,
        { CHIP8_OP_NONE, CHIP8_OP_NONE, CHIP8_OP_NONE }
    },
    { // SCR -> 00FB
        "SCR",
        CHIP8_OP_SCR,
        CHIP8_OP_MASK_WRD,
        CHIP8_OPR_IX,
        { CHIP8_OP_NONE, CHIP8_OP_NONE, CHIP8_OP_NONE }
    },
    { // SCL -> 00FC
        "SCL",
        CHIP8_OP_SCL,
        CHIP8_OP_MASK_WRD,
        CHIP8_OPR_IX,
        { CHIP8_OP_NONE, CHIP8_OP_NONE, CHIP8_OP_NONE }
    },
    { // EXIT -> 00FD
        "EXIT",
        CHIP8_OP_EXIT,
        CHIP8_OP_MASK_WRD,
        CHIP8_OPR_IX,
        { CHIP8_OP_NONE, CHIP8_OP_NONE, CHIP8_OP_NONE }
    },
    { // LOW -> 00FE
        "LOW",
        CHIP8_OP_LOW,
        CHIP8_OP_MASK_WRD,
        CHIP8_OPR_IX,
        { CHIP8_OP_NONE, CHIP8_OP_NONE, CHIP8_OP_NONE }
    },
    { // HIGH -> 00FF
        "HIGH",
        CHIP8_OP_HIGH,
        CHIP8_OP_MASK_WRD,
        CHIP8_OPR_IX,
        { CHIP8_OP_NONE, CHIP8_OP_NONE, CHIP8_OP_NONE }
    },
    { // SCD nibble -> 00CN
        "SCD",
        CHIP8_OP_SCD,
        CHIP8_OP_MASK_GSS,
        CHIP8_OPR_IX,
        { CHIP8_OP_NIBBLE, CHIP8_OP_NONE, CHIP8_OP_NONE }
    },
    { // JP addr -> 1NNN
        "JP",
        CHIP8_OP_JP,
        CHIP8_OP_MASK_GSN,
        CHIP8_OPR_IX,
        { CHIP8_OP_SLAB, CHIP8_OP_NONE, CHIP8_OP_NONE }
    },
    { // CALL addr -> 2NNN
        "CALL",
        CHIP8_OP_CALL,
        CHIP8_OP_MASK_GSN,
        CHIP8_OPR_IX,
        { CHIP8_OP_SLAB, CHIP8_OP_NONE, CHIP8_OP_NONE }
    },
    { // LD I, addr -> ANNN
        "LD",
        CHIP8_OP_LD_ADDR,
        CHIP8_OP_MASK_GSN,
        CHIP8_OPR_IX,
        { CHIP8_OP_IX, CHIP8_OP_SLAB, CHIP8_OP_NONE }
    },
    { // JP V0, addr
        "JP",
        CHIP8_OP_JP_V0,
        CHIP8_OP_MASK_GSN,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG0, CHIP8_OP_SLAB, CHIP8_OP_NONE }
    },
    { // SE Vx, byte -> 3XNN
        "SE",
        CHIP8_OP_SE_BYTE,
        CHIP8_OP_MASK_GSN,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_BYTE, CHIP8_OP_NONE }
    },
    { // SNE Vx, byte -> 4XNN
        "SNE",
        CHIP8_OP_SNE_BYTE,
        CHIP8_OP_MASK_GSN,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_BYTE, CHIP8_OP_NONE }
    },
    { // LD Vx, byte -> 6XNN
        "LD",
        CHIP8_OP_LD_BYTE,
        CHIP8_OP_MASK_GSN,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_BYTE, CHIP8_OP_NONE }
    },
    { // ADD Vx, byte -> 7XNN
        "ADD",
        CHIP8_OP_ADD_BYTE,
        CHIP8_OP_MASK_GSN,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_BYTE, CHIP8_OP_NONE }
    },
    { // RND Vx, byte -> CXNN
        "RND",
        CHIP8_OP_RND_BYTE,
        CHIP8_OP_MASK_GSN,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_BYTE, CHIP8_OP_NONE }
    },
    { // SE Vx, Vy -> 5XY0
        "SE",
        CHIP8_OP_SE_REG,
        CHIP8_OP_MASK_EXT,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_REG4, CHIP8_OP_NONE }
    },
    { // LD Vx, Vy -> 8XY0
        "LD",
        CHIP8_OP_LD_REG,
        CHIP8_OP_MASK_EXT,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_REG4, CHIP8_OP_NONE }
    },
    { // OR Vx, Vy -> 8XY1
        "OR",
        CHIP8_OP_OR,
        CHIP8_OP_MASK_EXT,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_REG4, CHIP8_OP_NONE }
    },
    { // AND Vx, Vy -> 8XY2
        "AND",
        CHIP8_OP_AND,
        CHIP8_OP_MASK_EXT,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_REG4, CHIP8_OP_NONE }
    },
    { // X0R Vx, Vy -> 8XY3
        "XOR",
        CHIP8_OP_XOR,
        CHIP8_OP_MASK_EXT,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_REG4, CHIP8_OP_NONE }
    },
    { // ADD Vx, Vy -> 8XY4
        "ADD",
        CHIP8_OP_ADD_REG,
        CHIP8_OP_MASK_EXT,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_REG4, CHIP8_OP_NONE }
    },
    { // SUB Vx, Vy -> 8XY5
        "SUB",
        CHIP8_OP_SUB,
        CHIP8_OP_MASK_EXT,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_REG4, CHIP8_OP_NONE }
    },
    { // SHR Vx -> 8XY6
        "SHR",
        CHIP8_OP_SHR,
        CHIP8_OP_MASK_EXT,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_REG4, CHIP8_OP_NONE }
    },
    { // SUBN Vx, Vy -> 8XY7
        "SUBN",
        CHIP8_OP_SUBN,
        CHIP8_OP_MASK_EXT,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_REG4, CHIP8_OP_NONE }
    },
    { // SHL Vx -> 8XYE
        "SHL",
        CHIP8_OP_SHL,
        CHIP8_OP_MASK_EXT,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_REG4, CHIP8_OP_NONE }
    },
    { // SNE Vx, Vy -> 9XY0
        "SNE",
        CHIP8_OP_SNE_REG,
        CHIP8_OP_MASK_EXT,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_REG4, CHIP8_OP_NONE }
    },
    { // SKP Vx -> EX9E
        "SKP",
        CHIP8_OP_SKP,
        CHIP8_OP_MASK_EXX,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_NONE, CHIP8_OP_NONE }
    },
    { // SKNP Vx -> EXA1
        "SKNP",
        CHIP8_OP_SKNP,
        CHIP8_OP_MASK_EXX,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_NONE, CHIP8_OP_NONE }
    },
    { // LD Vx, DT -> FX07
        "LD",
        CHIP8_OP_LD_DT,
        CHIP8_OP_MASK_EXX,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_DT, CHIP8_OP_NONE }
    },
    { // LD Vx, K -> FX0A
        "LD",
        CHIP8_OP_LD_KEY,
        CHIP8_OP_MASK_EXX,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_KEY, CHIP8_OP_NONE }
    },
    { // LD DT, Vx -> FX15
        "LD",
        CHIP8_OP_LD_REG_DT,
        CHIP8_OP_MASK_EXX,
        CHIP8_OPR_IX,
        { CHIP8_OP_DT, CHIP8_OP_REG8, CHIP8_OP_NONE }
    },
    { // LD ST, Vx -> FX18
        "LD",
        CHIP8_OP_LD_REG_ST,
        CHIP8_OP_MASK_EXX,
        CHIP8_OPR_IX,
        { CHIP8_OP_ST, CHIP8_OP_REG8, CHIP8_OP_NONE }
    },
    { // ADD I, Vx -> FX1E
        "ADD",
        CHIP8_OP_ADD_REG_IX,
        CHIP8_OP_MASK_EXX,
        CHIP8_OPR_IX,
        { CHIP8_OP_IX, CHIP8_OP_REG8, CHIP8_OP_NONE }
    },
    { // LD F, Vx -> FX29
        "LD",
        CHIP8_OP_LD_SPRITE,
        CHIP8_OP_MASK_EXX,
        CHIP8_OPR_IX,
        { CHIP8_OP_SPRITE, CHIP8_OP_REG8, CHIP8_OP_NONE }
    },
    { // LD B, Vx -> FX33
        "LD",
        CHIP8_OP_LDS_BCD,
        CHIP8_OP_MASK_EXX,
        CHIP8_OPR_IX,
        { CHIP8_OP_BCD, CHIP8_OP_REG8, CHIP8_OP_NONE }
    },
    { // LD [I], Vx -> FX55
        "LD",
        CHIP8_OP_LDS_REGS,
        CHIP8_OP_MASK_EXX,
        CHIP8_OPR_IX,
        { CHIP8_OP_IXR, CHIP8_OP_REG8, CHIP8_OP_NONE }
    },
    { // LD Vx, [I] -> FX65
        "LD",
        CHIP8_OP_LD_REGS,
        CHIP8_OP_MASK_EXX,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_IXR, CHIP8_OP_NONE }
    },
    { // DRW Vx, Vy, 0 -> DXY0
        "DRW",
        CHIP8_OP_DRW_EXT,
        CHIP8_OP_MASK_EXT,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_REG4, CHIP8_OP_NULL }
    },
    { // DRW Vx, Vy, nibble -> DXYN
        "DRW",
        CHIP8_OP_DRW_NIBBLE,
        CHIP8_OP_MASK_GSN,
        CHIP8_OPR_IX,
        { CHIP8_OP_REG8, CHIP8_OP_REG4, CHIP8_OP_NIBBLE }
    },
    { // DB byte -> NN -- assembler directive
        "DB",
        CHIP8_OP_DB,
        CHIP8_OP_MASK_GSB,
        CHIP8_OPR_DR,
        { CHIP8_OP_BYTE, CHIP8_OP_NONE, CHIP8_OP_NONE }
    },
    { // Sentinal to indicate end of optab
        NULL, 0, 0, 0, { 0 }
    }
};

/**
 * reserved symbols
 */
char asm_reserved[][4] = {
    "V0", "V1", "V2", "V3", "V4", "V5", "V6", "V7", "V8", "V9",
    "VA", "VB", "VC", "VD", "VE", "VF", "DT", "ST", "I", "[I]",
    "F", "B", "K", "",
};

#endif // _CHIP8_H
