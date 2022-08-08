#include "chip-8.h"

void chip8::initialize() {
    // resets all values to where they should be
    clear_memory();
    pc = PROGRAM_START;

    clear_stack();
    sp = 0;

    clear_registers();
    I = 0;

    delay = 0;
    sound = 0;

    draw = false;
    clear_display();
    clear_keyboard();

    // load the font
    for (int i = 0; i < SPRITE_WIDTH * NUM_KEYS; ++i) {
        memory[FONT_START + i] = font[i];
    }
}

bool chip8::load_program(std::string filename) {
    initialize();

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
        memory[i] = buffer[i - PROGRAM_START];
    }

    delete[] buffer;
    return true;
}

void chip8::execute_cycle() {
    uint16_t instruction = (memory[pc] << 8) | memory[pc + 1];    
    
    // increment program counter
    pc += 2;
    
    // execute the instruction 
    execute_instruction(instruction);

    // decrement the timers
    if (delay > 0) {
        delay--;
    }

    if (sound > 0) {
        sound--;
    }
}

void chip8::draw_sprite(uint8_t x, uint8_t y, uint8_t *sprite, uint8_t size) {
    this->registers[0xF] = 0;

    for (uint8_t i = 0; i < size; ++i) {
        for (uint8_t j = 0; j < 8; ++j) {
            bool pixel = (sprite[i] >> (7 - j)) & 0x01;
            uint8_t pos_x = (x + j) % DISPLAY_WIDTH;
            uint8_t pos_y = (y + i) % DISPLAY_HEIGHT;
            bool pixel_on = display[pos_x + pos_y * DISPLAY_WIDTH] == 0xFFFFFFFF;
            
            if (pixel_on && pixel) {
                registers[0xF] = 1; // pixel is erased
            }

            pixel = pixel ^ pixel_on;
            if (pixel != pixel_on) {
                // pixel at pos_x,pos_y changed
                draw = true;
            }

            display[pos_x + pos_y * DISPLAY_WIDTH] = pixel ? 0xFFFFFFFF : 0x0;
        }
    }
}

void chip8::clear_display() {
    for (uint8_t i = 0; i < DISPLAY_WIDTH; ++i) {
        for (uint8_t j = 0; j < DISPLAY_HEIGHT; ++j) {
            display[i + j * DISPLAY_WIDTH] = 0;
        }
    }
    // draw the display
    draw = true;
}

void chip8::clear_keyboard() {
    for (uint16_t i = 0; i < NUM_KEYS; ++i) {
        keyboard[i] = 0;
    }
}

void chip8::clear_memory() {
    for (uint16_t i = 0; i < MEMORY_SIZE; ++i) {
        memory[i] = 0;
    }
}

void chip8::clear_stack() {
    for (uint8_t i = 0; i < STACK_SIZE; ++i) {
        stack[i] = 0;
    }
}

void chip8::clear_registers() {
    for (uint8_t i = 0; i < NUM_REGISTERS; ++i) {
        registers[i] = 0;
    }
}

void chip8::execute_instruction(uint16_t instruction) {
    // make unique vals that are shifted and ready to be used
    uint8_t msb = instruction >> 12;         // X000

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
                    this->registers[0xF] = this->registers[b] >> 7; 
                    this->registers[a] = this->registers[b] << 1;
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
                    if (this->keyboard[registers[ind]] == 1) {
                        this->pc += 2;
                    }
                    break;

                case 0xA1:
                    if (this->keyboard[registers[ind]] == 0) {
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
                    this->I = a * 5 + FONT_START;
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
                    this->I = this->I + a + 1;
                    break;
                    
                case 0x65:
                    for (uint8_t i = 0; i <= a; ++i) {
                        this->registers[i] = this->memory[this->I + i];
                    }
                    this->I = this->I + a + 1;
                    break;
            }
            break;

        default:
            std::cout << "unknown instruction " << instruction << std::endl;
            break;
    }
}

