#include "chip-8.h"

chip8::chip8() {
    this->memory = new uint8_t[MEMORY_SIZE];
    this->stack = new uint16_t[STACK_SIZE];
    this->registers = new uint8_t[REGISTER_SIZE];
    this->display = new bool*[DISPLAY_WIDTH];
    for (uint8_t i = 0; i < DISPLAY_WIDTH; ++i) {
        this->display[i] = new bool[DISPLAY_HEIGHT];
    }
    this->keyboard = new bool[NUM_KEYS];

    this->distribution = std::uniform_int_distribution<int> (0x0, 0xFF);

    this->initialize();
}

void chip8::initialize() {
    // clear all the arrays
    this->draw = false;
    this->clear_display();
    this->clear_keyboard();
    this->clear_memory();
    this->load_font();
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

    if (program_file) {
        size = program_file.tellg();

        program_file.seekg(0, std::ios::beg);
        buffer = new uint8_t[size];
        program_file.read((char *)buffer, size);

        program_file.close();
    } else {
        return false;
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

void chip8::load_font() {
    uint8_t font [16 * 5] = {
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

    for (uint8_t i = 8; i < 16 * 5; ++i) {
        this->memory[i] = font[i];
    }
}

void chip8::execute_cycle() {
    uint16_t instruction = (this->memory[this->pc] << 8) | 
                            this->memory[this->pc + 1];    

    // std::cout << std::hex << instruction << std::endl;
    
    // increment program counter
    this->pc += 2;
    
    // figure out what to do with the instruction
    this->execute_instruction(instruction);

    // deal with the timers
    if (this->delay > 0) {
        this->delay--;
    }

    if (this->sound > 0) {
        this->sound--;
    }
}

void chip8::draw_sprite(uint8_t x, uint8_t y, uint8_t *sprite, uint8_t size) {
    this->registers[0xF] = 0;

    for (uint8_t i = 0; i < size; ++i) {
        for (uint8_t j = 0; j < 8; ++j) {
            bool pixel = (sprite[i] >> (7 - j)) & 0x01;
            uint8_t pos_x = (x + j) % DISPLAY_WIDTH;
            uint8_t pos_y = (y + i) % DISPLAY_HEIGHT;
            
            if (this->display[pos_x][pos_y] && pixel) {
                this->registers[0xF] = 1; // pixel is erased
            }

            pixel = pixel ^ this->display[pos_x][pos_y];
            if (pixel != this->display[pos_x][pos_y]) {
                // pixel at pos_x,pos_y changed
                this->set_draw_flag(true);
            }

            this->display[pos_x][pos_y] = pixel;
        }
    }
}

void chip8::clear_display() {
    for (uint8_t i = 0; i < DISPLAY_WIDTH; ++i) {
        for (uint8_t j = 0; j < DISPLAY_HEIGHT; ++j) {
            this->display[i][j] = 0;
        }
    }
    // draw the display
    this->draw = true;
}

void chip8::draw_display() {
    for (uint8_t i = 0; i < DISPLAY_HEIGHT; ++i) {
        for (uint8_t j = 0; j < DISPLAY_WIDTH; ++j) {
            std::cout << this->display[j][i];
        }
        std::cout << std::endl;
    }
}

bool **chip8::get_display() {
    return this->display;
}

bool chip8::get_draw_flag() {
    return this->draw;
}

void chip8::set_draw_flag(bool val) {
    this->draw = val;
}

bool *chip8::get_keyboard() {
    return this->keyboard;
}

void chip8::clear_keyboard() {
    for (uint16_t i = 0; i < NUM_KEYS; ++i) {
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

void chip8::execute_instruction(uint16_t instruction) {
    uint8_t msb = instruction >> 12;

    // make unique vals that are shifted and ready to be used
    uint8_t ind = 0x0F & (instruction >> 8); // 0X00
    uint8_t val = 0x00FF & instruction;      // 00XX
    uint8_t a = ind;                         // 0X00
    uint8_t b = 0x00F & (instruction >> 4);  // 00X0
    uint8_t tail = 0x000F & instruction;     // 000X

    switch (msb) {
        case 0:
            switch (0x0FFF & instruction) {
                case 0x0E0:
                    this->clear_display();
                    break;
                case 0x0EE:
                    this->sp--;
                    this->pc = this->stack[this->sp];
                    if (this->pc % 2 == 1) {
                        std::cout << "0" << std::endl;
                    }
                    break;
            }
            break;

        case 1:
            this->pc = 0x0FFF & instruction;
            if (this->pc % 2 == 1) {
                std::cout << "1" << std::endl;
            }
            break;

        case 2:
            this->stack[sp] = this->pc;
            this->sp++;
            this->pc = 0x0FFF & instruction;
            if (this->pc % 2 == 1) {
                std::cout << "2" << std::endl;
            }
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
            this->registers[ind] = this->registers[ind] % 0x100;
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
                    this->registers[0xF] = res > 0x100;
                    this->registers[a] = res % 0x100;
                    break;
                }
                case 5:
                    this->registers[0xF] = this->registers[a] > this->registers[b];
                    this->registers[a] -= this->registers[b];
                    break;
                case 6:
                    this->registers[0xF] = this->registers[ind] & 0x1;
                    this->registers[ind] = this->registers[ind] >> 1;
                    break;
                case 7:
                    this->registers[0xF] = this->registers[b] > this->registers[a];
                    this->registers[a] = this->registers[b] - this->registers[a];
                    break;
                case 0xE:
                    this->registers[0xF] = this->registers[ind] >> 7; 
                    this->registers[ind] = this->registers[ind] << 1;
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
            if (this->pc % 2 == 1) {
                std::cout << "B" << std::endl;
            }
            break;

        case 0xC:
            this->registers[ind] = ((uint8_t) this->distribution(this->generator)) & val;
            break;

        case 0xD:
        {
            uint8_t *sprite = new uint8_t[tail];

            for (uint8_t i = 0; i < tail; ++i) {
                sprite[i] = this->memory[this->I + i];
            }

            this->draw_sprite(this->registers[a], this->registers[b], sprite, tail);

            delete[] sprite;
            break;
        }

        case 0xE:
            switch (val) {
                case 0x9E:
                    if (this->keyboard[ind] == 1) {
                        this->pc += 2;
                    }
                    break;

                case 0xA1:
                    if (this->keyboard[ind] == 0) {
                        this->pc += 2;
                    }
                    break;
            }
            break;

        case 0xF:
            switch (val) {
                case 0x07:
                    this->registers[ind] = this->delay;
                    break;

                case 0x0A: 
                {
                    bool set_key = false;
                    for (uint8_t i = 0; i < NUM_KEYS; ++i) {
                        if (this->keyboard[i] == 1) {
                            this->registers[ind] = i;
                            set_key = true;
                            break;
                        }
                    }

                    if (!set_key) {
                        this->pc -= 2;
                    }
                    break;
                }

                case 0x15:
                    this->delay = this->registers[ind];
                    break;

                case 0x18:
                    this->sound = this->registers[ind];
                    break;

                case 0x1E:
                    this->I = this->registers[ind] + this->I;
                    break;

                case 0x29:
                    this->I = a * 5;
                    break;

                case 0x33:
                    this->memory[this->I] = (this->registers[ind] / 100) % 10;
                    this->memory[this->I + 1] = (this->registers[ind] / 10) % 10;
                    this->memory[this->I + 2] = this->registers[ind] % 10;
                    break;

                case 0x55:
                    for (uint8_t i = 0; i <= a; ++i) {
                        this->memory[this->I + i] = this->registers[i];
                    }
                    break;
                    
                case 0x65:
                    for (uint8_t i = 0; i <= a; ++i) {
                        this->registers[i] = this->memory[this->I + i];
                    }
                    break;
            }
            break;

        default:
            std::cout << "unknown instruction " << instruction << std::endl;
            break;
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

