/************************************
 * chip8.c - implementation for CHIP-8 interpreter
 *
 * Developer: Victor Nwosu
 ***********************************/

#include <stdio.h>

#include "chip8.h"

/**
 * load program from file into main memory
 * and return the number of bytes read
 * essentially the size of the program
 */
uint16_t load_program(const char *path);

void decode(uint16_t *opcode, uint16_t *slab, uint8_t *byte, uint8_t *regs);
void execute(uint16_t opcode, uint16_t slab, uint8_t byte, uint8_t regs);

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: %s FILE", argv[0]);
        return 1;
    }

    if (load_program(argv[1]) == 0) {
        printf("Error loading program");
        return 2;
    }

    rs2[CHIP8_SP] = 0;
    rs2[CHIP8_PC] = 0;
    rs2[CHIP8_IX] = 0;

    uint16_t *bits = (uint16_t *) memory;

    uint16_t opcode = 0x0000, slab = 0x000;
    uint8_t regs = 0xFF, byte = 0x00;

    // 0x00FD instruction exits the program
    while (opcode != 0xFFFF) { 
        // fetch
        rs2[CHIP8_IX] = *(bits + rs2[CHIP8_PC]++);

        // decode
        decode(&opcode, &slab, &byte, &regs);
        printf("0x%-10x | 0x%-10x | 0x%-10x | 0x%-10x | 0x%-10x | 0x%-10x\n", rs2[CHIP8_PC] - 1, rs2[CHIP8_IX], opcode, slab, byte, regs);

        execute(opcode, slab, byte, regs);
    }
}

uint16_t load_program(const char *path) {
    FILE *program = fopen(path, "rb");

    if (program == NULL) {
        return 0;
    }

    return fread(memory, sizeof(uint8_t), CHIP8_MEMORY_CAPACITY, program);
}

void decode(uint16_t *opcode, uint16_t *slab, uint8_t *byte, uint8_t *regs) {
    *regs = 0xFF;
    uint16_t opc = 0xFFFF;

    for (int i = 0; optab[i].mnemonic != NULL; i++) {
        if (opc == *opcode) {
            break;
        }

        opc = optab[i].mask & rs2[CHIP8_IX];
        printf("0x%-10x | 0x%-10x | 0x%-10x\n", optab[i].mask, rs2[CHIP8_IX], opc);


        switch (opc) {
            case CHIP8_OP_CLS:            // 0x00E0
            case CHIP8_OP_RET:            // 0x00EE
            case CHIP8_OP_SCR:            // 0x00FB
            case CHIP8_OP_SCL:            // 0x00FC
            case CHIP8_OP_EXIT:           // 0x00FD
            case CHIP8_OP_LOW:            // 0x00FE
            case CHIP8_OP_HIGH:           // 0x00FF
                *opcode = opc;
                break;
            case CHIP8_OP_JP:             // 0x1000
            case CHIP8_OP_CALL:           // 0x2000
            case CHIP8_OP_LD_ADDR:        // 0xA000
            case CHIP8_OP_JP_V0:          // 0xB000
                *slab = (optab[i].mask ^ 0xFFFF) & rs2[CHIP8_IX];
                *opcode = opc;
                break;
            case CHIP8_OP_SE_BYTE:        // 0x3000
            case CHIP8_OP_SNE_BYTE:       // 0x4000
            case CHIP8_OP_LD_BYTE:        // 0x6000
            case CHIP8_OP_ADD_BYTE:       // 0x7000
            case CHIP8_OP_RND_BYTE:       // 0xC000
                *byte = 0x00FF & rs2[CHIP8_IX];
                *regs = ((0x0F00 & rs2[CHIP8_IX]) >> 4) | 0x0F;
                *opcode = opc;
                break;
            case CHIP8_OP_SE_REG:         // 0x5000
            case CHIP8_OP_LD_REG:         // 0x8000
            case CHIP8_OP_OR:             // 0x8001
            case CHIP8_OP_AND:            // 0x8002
            case CHIP8_OP_XOR:            // 0x8003
            case CHIP8_OP_ADD_REG:        // 0x8004
            case CHIP8_OP_SUB:            // 0x8005
            case CHIP8_OP_SHR:            // 0x8006
            case CHIP8_OP_SUBN:           // 0x8007
            case CHIP8_OP_SHL:            // 0x800E
            case CHIP8_OP_SNE_REG:        // 0x9000
                *regs = (0x0FF0 & rs2[CHIP8_IX]) >> 4;
                *opcode = opc;
                break;
            case CHIP8_OP_SKP:            // 0xE09E
            case CHIP8_OP_SKNP:           // 0xE0A1
            case CHIP8_OP_LD_DT:          // 0xF007
            case CHIP8_OP_LD_KEY:         // 0xF00A
            case CHIP8_OP_LD_REG_DT:      // 0xF015
            case CHIP8_OP_LD_REG_ST:      // 0xF018
            case CHIP8_OP_ADD_REG_IX:     // 0xF01E
            case CHIP8_OP_LD_SPRITE:      // 0xF029
            case CHIP8_OP_LDS_BCD:        // 0xF033
            case CHIP8_OP_LDS_REGS:       // 0xF055
            case CHIP8_OP_LD_REGS:        // 0xF065
                *regs = ((0x0F00 & rs2[CHIP8_IX]) >> 4) | 0x0F;
                *opcode = opc;
                break;
            case CHIP8_OP_SCD:            // 0x00C0
                *byte = 0x000F & rs2[CHIP8_IX];
                *opcode = opc;
                break;
            case CHIP8_OP_DRW_NIBBLE:     // 0xD000
                *byte = 0x000F & rs2[CHIP8_IX];
                *regs = (0x0FF0 & rs2[CHIP8_IX]) >> 4;
                *opcode = opc;
                break;
            default:                      // Illegal instruction
                *opcode = 0xFFFF;
                *slab = 0xFFF;
                *byte = 0xFF;
                break;
        }
    }
}

void execute(uint16_t opcode, uint16_t slab, uint8_t byte, uint8_t regs) {
    switch (opcode) {
        case CHIP8_OP_CLS:            // 0x00E0
        case CHIP8_OP_RET:            // 0x00EE
        case CHIP8_OP_SCR:            // 0x00FB
        case CHIP8_OP_SCL:            // 0x00FC
        case CHIP8_OP_LOW:            // 0x00FE
        case CHIP8_OP_HIGH:           // 0x00FF
            break;
        case CHIP8_OP_JP:             // 0x1000
        case CHIP8_OP_CALL:           // 0x2000
        case CHIP8_OP_LD_ADDR:        // 0xA000
        case CHIP8_OP_JP_V0:          // 0xB000
            break;
        case CHIP8_OP_SE_BYTE:        // 0x3000
        case CHIP8_OP_SNE_BYTE:       // 0x4000
        case CHIP8_OP_LD_BYTE:        // 0x6000
        case CHIP8_OP_ADD_BYTE:       // 0x7000
        case CHIP8_OP_RND_BYTE:       // 0xC000
            break;
        case CHIP8_OP_SE_REG:         // 0x5000
        case CHIP8_OP_LD_REG:         // 0x8000
        case CHIP8_OP_OR:             // 0x8001
        case CHIP8_OP_AND:            // 0x8002
        case CHIP8_OP_XOR:            // 0x8003
        case CHIP8_OP_ADD_REG:        // 0x8004
        case CHIP8_OP_SUB:            // 0x8005
        case CHIP8_OP_SHR:            // 0x8006
        case CHIP8_OP_SUBN:           // 0x8007
        case CHIP8_OP_SHL:            // 0x800E
        case CHIP8_OP_SNE_REG:        // 0x9000
            break;
        case CHIP8_OP_SKP:            // 0xE09E
        case CHIP8_OP_SKNP:           // 0xE0A1
        case CHIP8_OP_LD_DT:          // 0xF007
        case CHIP8_OP_LD_KEY:         // 0xF00A
        case CHIP8_OP_LD_REG_DT:      // 0xF015
        case CHIP8_OP_LD_REG_ST:      // 0xF018
        case CHIP8_OP_ADD_REG_IX:     // 0xF01E
        case CHIP8_OP_LD_SPRITE:      // 0xF029
        case CHIP8_OP_LDS_BCD:        // 0xF033
        case CHIP8_OP_LDS_REGS:       // 0xF055
        case CHIP8_OP_LD_REGS:        // 0xF065
            break;
        case CHIP8_OP_SCD:            // 0x00C0
            break;
        case CHIP8_OP_DRW_NIBBLE:     // 0xD000
            break;
    }
}
