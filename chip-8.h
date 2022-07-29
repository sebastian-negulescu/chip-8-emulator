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

#define DISPLAY_WIDTH 0x40
#define DISPLAY_HEIGHT 0x20

#define NUM_KEYS 0x10

#ifndef CHIP8
#define CHIP8
class chip8 {
private:
    // memory structures
    uint8_t memory [MEMORY_SIZE];
    uint16_t pc;

    // stack structures
    uint16_t stack [STACK_SIZE];
    uint8_t sp;

    // register structures
    uint8_t registers [NUM_REGISTERS];
    uint16_t I;

    // timers
    uint8_t delay;
    uint8_t sound;

    // peripheral structures
    bool draw;
    bool display [DISPLAY_WIDTH * DISPLAY_HEIGHT];
    bool keyboard [NUM_KEYS];

    // random byte generator
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution;

    void clear_display();
    void clear_keyboard();
    void clear_memory();
    void clear_stack();
    void clear_registers();
    
    void load_font();

    void execute_instruction(uint16_t instruction);
    void draw_sprite(uint8_t x, uint8_t y, uint8_t *sprite, uint8_t size);

public:
    chip8();
    ~chip8();

    bool **get_display();
    bool get_draw_flag();
    void set_draw_flag(bool val);
    bool *get_keyboard();

    void draw_display();

    void initialize();
    bool load_program(std::string filename);
    void execute_cycle();
};
#endif
