#include "chip-8.h"

chip8::chip8() {
    this->memory = new uint8_t[MEMORY_SIZE];
    this->stack = new uint16_t[STACK_SIZE];
    this->registers = new uint8_t[REGISTER_SIZE];
    this->display = new bool*[DISPLAY_WIDTH];
    for (uint8_t i = 0; i < DISPLAY_WIDTH; ++i) {
        this->display[i] = new bool[DISPLAY_HEIGHT];
    }
    this->keyboard = new bool[KEYS];

    this->initialize();
}

void chip8::initialize() {
    // clear all the arrays
    this->clear_display();
    this->clear_keyboard();
    this->clear_memory();
    this->clear_stack();
    this->clear_registers();

    this->pc = PROGRAM_START;
    this->sp = 0;

    this->I = 0;

    this->delay = 0;
    this->sound = 0;
}

bool chip8::load_program(std::string filename) {
    this->initialize();

    // open file in binary and at end
    std::ifstream program_file(filename, std::ios::binary | std::ios::ate);
    long size = 0;
    uint8_t *buffer;

    size = program_file.tellg();

    program_file.seekg(0, std::ios::beg);
    buffer = new uint8_t[size];
    program_file.read((char *)buffer, size);

    program_file.close();

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
    
    // increment program counter
    this->pc += 2;
    
    // figure out what to do with the instruction
    this->execute_instruction(instruction);
}

void chip8::execute_instruction(uint16_t instruction) {
    std::cout << std::hex << instruction << std::endl;

    uint8_t msb = instruction >> 12;

    // make unique vals that are shifted and ready to be used
    uint8_t ind = 0x0F & (instruction >> 8);
    uint8_t val = 0x00FF & instruction;
    uint8_t a = ind;
    uint8_t b = 0x00F & (instruction >> 4);
    uint8_t tail = 0x000F & instruction;

    switch (msb) {
        case 0:
            switch (0x0FFF & instruction) {
                case 0x0E0:
                    this->clear_display();
                    break;
                case 0x0EE:
                    this->pc = this->stack[this->sp];
                    this->sp--;
                    break;
            }
            break;

        case 1:
            this->pc = 0x0FFF & instruction;
            break;

        case 2:
            this->stack[sp] = this->pc;
            this->sp++;
            this->pc = 0x0FFF & instruction;
            break;

        case 3:
            if (this->registers[ind] == val) {
                this->pc += 2;
            }
            break;

        case 4:
            if (this->registers[ind] != val) {
                this->pc += 2;
            }
            break;

        case 5:
            if (this->registers[a] == this->registers[b]) {
                this->pc += 2;
            }
            break;

        case 6:
            this->registers[ind] = val;
            break;

        case 7:
            this->registers[ind] += val;
            break;

        case 8:
            switch (tail) {
                case 0:
                    this->registers[a] = this->registers[b];
                    break;
                case 1:
                    this->registers[a] = this->registers[a] | this->registers[b];
                    break;
                case 2:
                    this->registers[a] = this->registers[a] & this->registers[b];
                    break;
                case 3:
                    this->registers[a] = this->registers[a] ^ this->registers[b];
                    break;
                case 4:
                {
                    uint16_t res = this->registers[a] + this->registers[b];
                    this->registers[a] = res & 0x00FF;
                    this->registers[0xF] = bool(res >> 8);
                    break;
                }
                case 5:
                    this->registers[0xF] = this->registers[a] > this->registers[b];
                    this->registers[a] -= this->registers[b];
                    break;
                case 6:
                    this->registers[0xF] = this->registers[a] & 0x01;
                    this->registers[a] = this->registers[a] >> 1;
                    break;
                case 7:
                    this->registers[0xF] = this->registers[b] > this->registers[a];
                    this->registers[a] = this->registers[b] - this->registers[a];
                    break;
                case 0xE:
                    this->registers[0xF] = this->registers[a] >> 7; 
                    this->registers[a] = this->registers[a] << 1;
                    break;
            }
            break;

        case 9:
            if (this->registers[a] != this->registers[b]) {
                this->pc += 2;
            }
            break;

        case 0xA:
            this->I = 0x0FFF & instruction;
            break;

        case 0xB:
            this->pc = this->registers[0] + (0x0FFF & instruction);
            break;

        case 0xC:
            // replace 1 with random byte
            this->registers[a] = 0x01 & val;
            break;

        case 0xD:
        {
            bool *sprite = new bool[tail * 8];

            for (uint8_t i = 0; i < tail; ++i) {
                uint8_t byte = this->memory[this->I + i]; 
                for (uint8_t j = 0; j < 8; ++j) {
                    sprite[i * 8 + j] = (byte << j) & 0x80;
                }
            }

            this->draw_sprite(a, b, sprite, tail * 8);

            delete[] sprite;
            break;
        }

        case 0xE:
            switch (val) {
                case 0x9E:
                    if (this->keyboard[a]) {
                        this->pc++;
                    }
                    break;

                case 0xA1:
                    if (!this->keyboard[a]) {
                        this->pc++;
                    }
                    break;
            }
            break;
        default:
            std::cout << "unknown instruction " << instruction << std::endl;
    }
}

void chip8::draw_sprite(uint8_t x, uint8_t y, bool *sprite, uint8_t size) {
    uint8_t cx = x;
    uint8_t cy = y;
    this->registers[0xF] = 0;

    for (uint8_t i = 0; i < size; ++i) {
        bool old_pixel = this->display[cx][cy];
        this->display[cx][cy] = this->display[cx][cy] ^ sprite[i]; 
    
        if (old_pixel == 1 && this->display[cx][cy] == 0) {
            // if the pixel was erased, update the VF register
            this->registers[0xF] = 1;
        }

        cx++;
        if (cx == DISPLAY_WIDTH) {
            cx = 0;
            cy++;
            if (cy == DISPLAY_HEIGHT) {
                cy = 0;
            }
        }
    }
}

void chip8::clear_display() {
    for (uint8_t i = 0; i < DISPLAY_WIDTH; ++i) {
        for (uint8_t j = 0; j < DISPLAY_HEIGHT; ++j) {
            this->display[i][j] = 0;
        }
    }
}

void chip8::clear_keyboard() {
    for (uint16_t i = 0; i < KEYS; ++i) {
        this->keyboard[i] = 0;
    }
}

void chip8::clear_memory() {
    for (uint16_t i = 0; i < MEMORY_SIZE; ++i) {
        this->memory[i] = 0;
    }
}

void chip8::clear_stack() {
    for (uint8_t i = 0; i < STACK_SIZE; ++i) {
        this->stack[i] = 0;
    }
}

void chip8::clear_registers() {
    for (uint8_t i = 0; i < REGISTER_SIZE; ++i) {
        this->registers[i] = 0;
    }
}

chip8::~chip8() {
    delete[] this->memory;
    delete[] this->stack;
    delete[] this->registers;

    for (uint8_t i = 0; i < DISPLAY_WIDTH; ++i) {
        delete[] this->display[i];
    }
    delete[] this->display;


}

