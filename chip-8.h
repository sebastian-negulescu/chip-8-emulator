#include <stdint.h>
#include <string>
#include <fstream>
#include <iostream>
#include <bitset>
#include <random>

// constants
#define MEMORY_SIZE 0x1000
#define STACK_SIZE 0x10
#define NUM_REGISTERS 0x10

#define PROGRAM_START 0x200

#define FONT_START 0x0
#define SPRITE_WIDTH 5

#define DISPLAY_WIDTH 0x40
#define DISPLAY_HEIGHT 0x20

#define NUM_KEYS 0x10

#ifndef CHIP8
#define CHIP8
class chip8 {
private:
    // memory structures
    uint8_t memory [MEMORY_SIZE]{};
    uint16_t pc;

    // stack structures
    uint16_t stack [STACK_SIZE]{};
    uint8_t sp;

    // register structures
    uint8_t registers [NUM_REGISTERS]{};
    uint16_t I;

    // timers
    uint8_t delay;
    uint8_t sound;

    // font
    const uint8_t font [SPRITE_WIDTH * NUM_KEYS] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    }; 

    // random byte generator
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution = 
        std::uniform_int_distribution<int> (0x0, 0xFF); // generate one byte

    void clear_display();
    void clear_keyboard();
    void clear_memory();
    void clear_stack();
    void clear_registers();
    
    void execute_instruction(uint16_t instruction);
    void draw_sprite(uint8_t x, uint8_t y, uint8_t *sprite, uint8_t size);

public:

    // peripheral structures
    bool draw;
    uint32_t display [DISPLAY_WIDTH * DISPLAY_HEIGHT]{};
    bool keyboard [NUM_KEYS]{};

    // functions around running the emulator
    void initialize();
    bool load_program(std::string filename);
    void execute_cycle();
};
#endif
