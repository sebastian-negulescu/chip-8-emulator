#include <stdint.h>
#include <string>
#include <fstream>
#include <iostream>
#include <bitset>

#define MEMORY_SIZE 0x1000
#define STACK_SIZE 0x10
#define REGISTER_SIZE 0x10

#define PROGRAM_START 0x200

#define DISPLAY_WIDTH 0x40
#define DISPLAY_HEIGHT 0x20

#define KEYS 0x10

#ifndef CHIP8
#define CHIP8
class chip8 {
private:
    uint8_t *memory;
    uint16_t pc;

    uint16_t *stack;
    uint8_t sp;

    uint8_t *registers;
    uint16_t I;

    uint8_t delay;
    uint8_t sound;

    bool **display;
    bool *keyboard;

    void clear_display();
    void clear_keyboard();
    void clear_memory();
    void clear_stack();
    void clear_registers();

    void execute_instruction(uint16_t instruction);
    void draw_sprite(uint8_t x, uint8_t y, bool *sprite, uint8_t size);

public:
    chip8();
    ~chip8();

    void initialize();
    bool load_program(std::string filename);
    void execute_cycle();
};
#endif
