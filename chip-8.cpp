#include "chip-8.h"

chip8::chip8() {
    this->memory = new uint8_t[MEMORY_SIZE];
    this->pc = PROGRAM_START;

    this->stack = new uint16_t[STACK_SIZE];
    this->sp = 0;

    this->registers = new uint8_t[REGISTER_SIZE];
    this->I = 0;

    this->delay = 0;
    this->sound = 0;
}

void chip8::initialize() {
    for (uint16_t i = 0; i < MEMORY_SIZE; ++i) {
        this->memory[i] = 0;
    }
    for (uint8_t i = 0; i < STACK_SIZE; ++i) {
        this->stack[i] = 0;
        this->registers[i] = 0;
    }

    this->pc = PROGRAM_START;
    this->sp = 0;

    this->I = 0;

    this->delay = 0;
    this->sound = 0;
}

bool chip8::load_program(std::string filename) {
    this->initialize();

    std::ifstream program_file(filename, std::ios::binary | std::ios::ate);
    long size = 0;
    uint8_t *buffer;
    if (program_file) {
        size = program_file.tellg();

        program_file.seekg(0, std::ios::beg);
        buffer = new uint8_t[size];
        program_file.read((char *)buffer, size);

        if (!program_file) {
            delete[] buffer;
            return false;
        }

        program_file.close();
    }

    // if size > 0x1000 - 0x200, then return false
    uint16_t memory_size = MEMORY_SIZE - PROGRAM_START;
    if (size > memory_size) {
        delete[] buffer;
        return false;
    }

    for (uint16_t i = PROGRAM_START; i < size + PROGRAM_START; ++i) {
        // store in memory starting from 0x200
        this->memory[i] = buffer[i - PROGRAM_START];
    }

    delete[] buffer;
    return true;
}

void chip8::execute_cycle() {
    uint16_t instruction = (this->memory[this->pc] << 8) | 
                            this->memory[this->pc + 1];    
    
    // figure out what to do with the instruction
    
    // pc will always be even
    if (this->pc + 2 < MEMORY_SIZE) {
        this->pc += 2;
    } else {
        // bad
        return;
    }
}

chip8::~chip8() {
    delete[] this->memory;
    delete[] this->stack;
    delete[] this->registers;
}

