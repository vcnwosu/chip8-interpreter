/************************************
 * chip8c.c - implementation of a less-than-modest
 *            CHIP-8 assembler
 *
 * Developer: Victor Nwosu
 ***********************************/

#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <unistd.h>

#include "chip8.h"

typedef struct {
    char symbol[32];
    int16_t address;
} chip8_symbol;

char * strip(char *line, ssize_t *linelen);
bool reserved(const char *symbol);
uint16_t assemble(const char *opr, const char *op1, const char *op2, const char *op3, chip8_symbol *symtab, int count);

bool parse(FILE *src, const char *sep, chip8_symbol **symtab, int *capacity, int *count);
bool build(FILE *src, FILE *dst, const char *sep, chip8_symbol *symtab, int count, bool build);

int main(int argc, char **argv) {
    if (argc < 2 || argc > 2) {
        printf("usage: %s <src>.ch8\n", argv[0]);
        return 1;
    }

    int infile_len = strlen(argv[1]);

    if (infile_len > 15) {
        printf("Filename too long. Max allowed characters is 15\n");
        return 2;
    }

    if (strncmp(*(argv + 1) + infile_len - 4, ".ch8", 4) != 0) {
        printf("Unrecognized file type\n");
        return 3;
    }
 
    char outfile_tmp[16] = {0};
    char outfile_bin[16] = {0};

    strncpy(outfile_tmp, argv[1], infile_len - 4);
    strncpy(outfile_tmp + infile_len - 4, ".tmp", 4);

    strncpy(outfile_bin, argv[1], infile_len - 4);
    outfile_bin[infile_len - 4] = '\0';

    FILE *src, *dst;

    if ((src = fopen(argv[1], "r")) == NULL) {
        printf("Error opening file\n");
        return 4;
    }

    if ((dst = fopen(outfile_tmp, "wb")) == NULL) {
        printf("Error opening temp output file\n");
        return 5;
    }

    chip8_symbol *symtab = NULL;
    int capacity = 0;
    int count = 0;
    char *sep = ",\t ";

    bool parsed = parse(src, sep, &symtab, &capacity, &count);
    bool built = build(src, dst, sep, symtab, count, parsed);

    free(symtab);

    fclose(dst);
    fclose(src);

    if (!built) {
        printf("Assembly translation failed. Please check"
            " your source code for syntax errors and verify"
            " that the correct assembler directives and target"
            " architecture are being used.\n"
        );

        return 6;
    }

    rename(outfile_tmp, outfile_bin);
    chmod(outfile_bin, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

    return 0;
}

bool reserved(const char *symbol) {
    for (int i = 0; optab[i].mnemonic != NULL; i++) {
        if (strcmp(symbol, optab[i].mnemonic) == 0) {
            return true;
        }
    }

    for (int i = 0; strcmp(asm_reserved[i], "") != 0; i++) {
        if (strcmp(symbol, asm_reserved[i]) == 0) {
            return true;
        }
    }

    return false;
}

char* strip(char *line, ssize_t *linelen) {
    while (*line != '\0' && *line == ' ') {
        line++;
    }

    char *start = line;

    while (*line != '\0' && *line != ';' && *line != '\n') {
        line++;
    }

    if (*line == ';' || *line == '\n') {
        while (*(--line) == ' ');
    }

    while (*(++line) != '\0') {
        *line = '\0';
    }

    *linelen = (ssize_t) strlen(start);

    return start;
}

uint16_t assemble(const char *opr, const char *op1, const char *op2, const char *op3, chip8_symbol *symtab, int count) {
    uint16_t translation = 0;
    const char *operands[] = { op1, op2, op3 }; 

    for (int i = 0; optab[i].mnemonic != NULL; i++) {
        if (strcmp(opr, optab[i].mnemonic) != 0) {
            continue;
        }

        translation = optab[i].opcode;

        for (int j = 0; j < 3; j++) {
            bool valid = true;

            switch (optab[i].operands[j]) {
                case CHIP8_OP_NONE: {
                    valid = operands[j] == NULL;
                    break;
                }
                case CHIP8_OP_REG0: {
                    valid = operands[j] != NULL && strcmp(operands[j], "V0") == 0;
                    break;
                }
                case CHIP8_OP_REG4:
                case CHIP8_OP_REG8: {
                    if (operands[j] == NULL || strncmp(operands[j], "V", 1) != 0 || strlen(operands[j]) > 2 || strcmp(operands[j], "VF") == 0) {
                        valid = false;
                        break;
                    }

                    errno = 0;
                    long val = strtol(*(operands + j) + 1, NULL, 16);

                    if (errno != 0) {
                        valid = false;
                        break;
                    }

                    translation |= CHIP8_OP_REG8 == optab[i].operands[j] 
                                    ? (uint16_t) (val << 8)
                                    : (uint16_t) (val << 4);

                    break;
                }
                case CHIP8_OP_NIBBLE:
                case CHIP8_OP_BYTE: {
                    if (operands[j] == NULL) {
                        valid = false;
                        break;
                    }

                    errno = 0;
                    long val = strtol(operands[j], NULL, 16);

                    if (errno != 0) {
                        valid = 0;
                        break;
                    }

                    valid = (CHIP8_OP_NIBBLE == optab[i].operands[j] && val <= 0xF)
                            || (CHIP8_OP_BYTE == optab[i].operands[j] && val <= 0xFF); 

                    if (!valid) {
                        break;
                    }

                    translation |= (uint16_t) val;

                    break;
                }
                case CHIP8_OP_SLAB: {
                    if (operands[j] == NULL) {
                        valid = false;
                        break;
                    }

                    errno = 0;
                    long val = -1;

                    for (int k = 0; k < count; k++) {
                        if (strcmp(symtab[k].symbol, operands[j]) == 0) {
                            val = symtab[k].address;
                        }
                    }

                    if (val == -1) {
                        val = strtol(operands[j], NULL, 16);

                        if (errno != 0) {
                            valid = false;
                            break;
                        }
                    }

                    translation |= (uint16_t) val;

                    break;
                }
                case CHIP8_OP_DT: {
                    valid = operands[j] != NULL && strcmp(operands[j], "DT") == 0;
                    break;
                }
                case CHIP8_OP_ST: {
                    valid = operands[j] != NULL && strcmp(operands[j], "ST") == 0;
                    break;
                }
                case CHIP8_OP_IX: {
                    valid = operands[j] != NULL && strcmp(operands[j], "I") == 0;
                    break;
                }
                case CHIP8_OP_IXR: {
                    valid = operands[j] != NULL && strcmp(operands[j], "[I]") == 0;
                    break;
                }
                case CHIP8_OP_BCD: {
                    valid = operands[j] != NULL && strcmp(operands[j], "B") == 0;
                    break;
                }
                case CHIP8_OP_SPRITE: {
                    valid = operands[j] != NULL && strcmp(operands[j], "F") == 0;
                    break; 
                }
                case CHIP8_OP_HF: {
                    valid = operands[j] != NULL && strcmp(operands[j], "HF") == 0;
                    break;
                }
                default:
                    valid = false;
                    break;
            }

            if (!valid) {
                translation = 0;
                break;
            }
        }

        if (translation != 0) {
            break;
        }
    }

    if (translation == 0) {
        return 0xFFFF;
    }

    return translation;
}

bool parse(FILE *src, const char *sep, chip8_symbol **symtab, int *capacity, int *count) {
    uint16_t address = 0x000;
    bool success = true;

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen = 0;

    while ((linelen = getline(&line, &linecap, src)) > 0) {
        char *stripped = strip(line, &linelen);

        if (linelen == 0) {
            continue;
        }

        if (stripped[linelen - 1] != ':') {
            address++;
            continue;
        }

        if (linelen - 1 > 30) {
            printf("Label %s exceeds maximum length of 32 characters\n", stripped);
            success = false;
            break;
        }

        stripped[--linelen] = '\0';

        if (reserved(stripped)) {
            printf("Label %s is a reserved symbol\n", stripped);
            success = false;
            break;
        }

        if (*count >= *capacity) {
            *capacity = (*capacity == 0) ? 2 : *capacity * 2;
            *symtab = realloc(*symtab, *capacity * sizeof(chip8_symbol));
        }

        strncpy((*symtab)[*count].symbol, stripped, 30);
        (*symtab)[*count].address = address;
        (*count)++;
    }

    free(line);

    return success;
}

bool build(FILE *src, FILE *dst, const char *sep, chip8_symbol *symtab, int count, bool build) {
    if (build == false) {
        return build;
    }

    char *line = NULL;
    size_t linecap = 0;
    ssize_t linelen = 0;

    clearerr(src);
    rewind(src);

    while ((linelen = getline(&line, &linecap, src)) > 0) {
        char *stripped = strip(line, &linelen);

        if (linelen == 0) {
            continue;
        }

        if (stripped[linelen - 1] == ':') {
            continue;
        }

        char stripped_copy[1024] = { 0 };
        strcpy(stripped_copy, stripped);

        char *token = strtok(stripped, sep);
        char *opr = token, *op1 = NULL, *op2 = NULL, *op3 = NULL;
        int opcount = 0;

        while (token != NULL) {
            token = strtok(NULL, sep);

            switch (++opcount) {
                case 1:
                    op1 = token;
                    break;
                case 2:
                    op2 = token;
                    break;
                case 3:
                    op3 = token;
                    break;
            }
        }

        uint16_t translation = assemble(opr, op1, op2, op3, symtab, count);

        if (translation == 0xFFFF) {
            printf("Unrecognized instruction or directive: %s\n", stripped_copy);
            build = false;
            break;
        }

        if (strcmp(opr, "DB") == 0) {
            uint8_t translation_cast = (uint8_t) translation;
            fwrite(&translation_cast, sizeof(uint8_t), 1, dst);
        } else {
            fwrite(&translation, sizeof(uint16_t), 1, dst);
        }

        fflush(dst);
    }

    free(line);

    return build;
}
