#include "chip-8.h"
#include "peripherals.h"

int main() {
    chip8 cpu;
    peripherals peripherals;

    bool loaded = cpu.load_program("test_opcode.ch8");

    if (!loaded) {
        return -1;
    }

    while (true) { 
        std::cin.get();
        cpu.execute_cycle();
        if (cpu.get_draw_flag()) {
            peripherals.draw_display(cpu.get_display(), DISPLAY_WIDTH, DISPLAY_HEIGHT);
            SDL_PumpEvents();
            cpu.set_draw_flag(false);
        }
    }
}
