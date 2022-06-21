#include "peripherals.h"

void peripherals::draw_display(bool **display, uint8_t display_width, uint8_t display_height) {
    SDL_SetRenderDrawColor(this->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(this->renderer);

    SDL_SetRenderDrawColor(this->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_Rect pixel = { 0, 0, this->width / display_width, this->height / display_height };
    for (uint8_t i = 0; i < display_height; ++i) {
        for (uint8_t j = 0; j < display_width; ++j) {
            if (display[j][i] == 1) {
                // render white square
                pixel.x = j * this->width / display_width;
                pixel.y = i * this->height / display_height;
                SDL_RenderFillRect(this->renderer, &pixel);                
            }
        }
    } 

    SDL_RenderPresent(this->renderer);
}

bool peripherals::process_input(bool *keyboard, uint8_t num_keys) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        // process the keyboard events
    }

    return 0;
}

peripherals::peripherals() {
    SDL_Init(SDL_INIT_VIDEO);

    this->width = 640;
    this->height = 480;

    this->window = SDL_CreateWindow(
        "chip 8 emulator",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        this->width,
        this->height,
        SDL_WINDOW_SHOWN
    );
    this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

peripherals::~peripherals() {
    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

