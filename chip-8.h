#include <stdint.h>
#include <string>
#include <fstream>
#include <iostream>
#include <bitset>

#define MEMORY_SIZE 0x1000
#define STACK_SIZE 0x10
#define REGISTER_SIZE 0x10

#define PROGRAM_START 0x200

#ifndef CHIP8
#define CHIP8
class chip8 {
    uint8_t *memory;
    uint16_t pc;

    uint16_t *stack;
    uint8_t sp;

    uint8_t *registers;
    uint16_t I;

    uint8_t delay;
    uint8_t sound;

public:
    chip8();
    ~chip8();

    void initialize();
    bool load_program(std::string filename);
    void execute_cycle();
};
#endif
