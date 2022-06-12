#include <stdint.h>

#ifndef CHIP8
#define CHIP8
class chip8 {
    uint8_t *memory;
    uint8_t *stack;
    uint8_t *registers;
    uint8_t pc;

public:
    chip8();
};
#endif
