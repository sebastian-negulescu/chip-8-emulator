#include "chip-8.h"

int main() {
    chip8 cpu;
    bool loaded = cpu.load_program("glitchGhost.ch8");

    if (!loaded) {
        return -1;
    }

    while (true) { 
        std::cin.get();
        cpu.execute_cycle();
    }
}
