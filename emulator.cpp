#include "chip-8.h"

int main() {
    chip8 cpu;
    std::cout << cpu.load_program("glitchGhost.ch8") << std::endl;
    cpu.execute_cycle();
}
