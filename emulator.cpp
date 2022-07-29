#include "chip-8.h"
#include "peripherals.h"
#include <chrono>

int main() {
    chip8 cpu;
    peripherals peripherals{DISPLAY_WIDTH * 10, DISPLAY_HEIGHT * 10, DISPLAY_WIDTH, DISPLAY_HEIGHT};

    bool loaded = cpu.load_program("glitchGhost.ch8");

    if (!loaded) {
        return -1;
    }

    bool quit = false;
    std::chrono::high_resolution_clock::time_point previous_time = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point current_time;
    double clock_cycle = 60; // hz

    while (!quit) { 
        quit = peripherals.process_input(cpu.get_keyboard());
        if (quit) {
            continue;
        }

        current_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> dt = std::chrono::duration_cast< std::chrono::duration<double> >(current_time - previous_time);
        if (dt.count() >= 1/clock_cycle) {
            cpu.execute_cycle();
            previous_time = current_time;
        }

        if (cpu.get_draw_flag()) {
            peripherals.draw_display(cpu.get_display(), DISPLAY_WIDTH, DISPLAY_HEIGHT);
            cpu.set_draw_flag(false);
        }
    }
}
