#include "chip-8.h"
#include "peripherals.h"
#include <chrono>

int main() {
    chip8 cpu{};
    peripherals peripherals{DISPLAY_WIDTH * 10, DISPLAY_HEIGHT * 10, DISPLAY_WIDTH, DISPLAY_HEIGHT};
    int pitch = sizeof(cpu.display[0]) * DISPLAY_WIDTH;

    bool loaded = cpu.load_program("./roms/glitchGhost.ch8");

    if (!loaded) {
        return -1;
    }

    bool quit = false;
    std::chrono::high_resolution_clock::time_point previous_time = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point current_time;
    double clock_cycle = 1;

    while (!quit) { 
        if (quit) {
            continue;
        }
        
        quit = peripherals.process_input(cpu.keyboard);

        current_time = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(current_time - previous_time).count();
        if (dt > clock_cycle) {
            previous_time = current_time;
            cpu.execute_cycle();
        }

        if (cpu.draw) {
            peripherals.draw_display(cpu.display, pitch);
            cpu.draw = false;
        }

    }
}
