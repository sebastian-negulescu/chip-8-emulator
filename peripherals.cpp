#include "peripherals.h"
#include <iostream>

void peripherals::draw_display(bool **processor_display, int display_width, int display_height) {
    // need to convert display to uint32_t
    for (int i = 0; i < this->display_height; ++i) {
        for (int j = 0; j < this->display_width; ++j) {
            this->display[i * this->display_width + j] = 0xFFFFFFFF * processor_display[j][i];
        }
    }

    SDL_UpdateTexture(this->texture, nullptr, this->display, this->pitch);
    SDL_RenderClear(this->renderer);
	SDL_RenderCopy(this->renderer, this->texture, nullptr, nullptr);
	SDL_RenderPresent(this->renderer);
}

bool peripherals::process_input(bool *keyboard) {
    SDL_Event event;
    bool quit = false;

    while (SDL_PollEvent(&event)) {
        // process the keyboard events
        switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break; 

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_1:
                        keyboard[0x1] = 1;
                        break;
                    case SDLK_2:
                        keyboard[0x2] = 1;
                        break;
                    case SDLK_3:
                        keyboard[0x3] = 1;
                        break;
                    case SDLK_4:
                        keyboard[0xC] = 1;
                        break;
                    case SDLK_q:
                        keyboard[0x4] = 1;
                        break;
                    case SDLK_w:
                        keyboard[0x5] = 1;
                        break;
                    case SDLK_e:
                        keyboard[0x6] = 1;
                        break;
                    case SDLK_r:
                        keyboard[0xD] = 1;
                        break;
                    case SDLK_a:
                        keyboard[0x7] = 1;
                        break;
                    case SDLK_s:
                        keyboard[0x8] = 1;
                        break;
                    case SDLK_d:
                        keyboard[0x9] = 1;
                        break;
                    case SDLK_f:
                        keyboard[0xE] = 1;
                        break;
                    case SDLK_z:
                        keyboard[0xA] = 1;
                        break;
                    case SDLK_x:
                        keyboard[0x0] = 1;
                        break;
                    case SDLK_c:
                        keyboard[0xB] = 1;
                        break;
                    case SDLK_v:
                        keyboard[0xF] = 1;
                        break;
                }
                break;

            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_1:
                        keyboard[0x1] = 0;
                        break;
                    case SDLK_2:
                        keyboard[0x2] = 0;
                        break;
                    case SDLK_3:
                        keyboard[0x3] = 0;
                        break;
                    case SDLK_4:
                        keyboard[0xC] = 0;
                        break;
                    case SDLK_q:
                        keyboard[0x4] = 0;
                        break;
                    case SDLK_w:
                        keyboard[0x5] = 0;
                        break;
                    case SDLK_e:
                        keyboard[0x6] = 0;
                        break;
                    case SDLK_r:
                        keyboard[0xD] = 0;
                        break;
                    case SDLK_a:
                        keyboard[0x7] = 0;
                        break;
                    case SDLK_s:
                        keyboard[0x8] = 0;
                        break;
                    case SDLK_d:
                        keyboard[0x9] = 0;
                        break;
                    case SDLK_f:
                        keyboard[0xE] = 0;
                        break;
                    case SDLK_z:
                        keyboard[0xA] = 0;
                        break;
                    case SDLK_x:
                        keyboard[0x0] = 0;
                        break;
                    case SDLK_c:
                        keyboard[0xB] = 0;
                        break;
                    case SDLK_v:
                        keyboard[0xF] = 0;
                        break;
                }
                break;
        }
    }

    return quit;
}

peripherals::peripherals(int window_width, int window_height, int display_width, int display_height) {
    SDL_Init(SDL_INIT_VIDEO);

    this->display_width = display_width;
    this->display_height = display_height;
    this->display = new uint32_t[display_width * display_height];
    this->pitch = sizeof(this->display[0]) * this->display_width;

    this->window = SDL_CreateWindow(
        "chip 8 emulator",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        window_width,
        window_height,
        SDL_WINDOW_SHOWN
    );
    this->renderer = SDL_CreateRenderer(
        this->window, 
        -1, 
        SDL_RENDERER_ACCELERATED
    );
    this->texture = SDL_CreateTexture(
        this->renderer, 
        SDL_PIXELFORMAT_RGBA8888, 
        SDL_TEXTUREACCESS_STREAMING, 
        display_width, 
        display_height
    );

}

peripherals::~peripherals() {
    delete[] this->display;

    SDL_DestroyRenderer(this->renderer);
    SDL_DestroyWindow(this->window);
    SDL_DestroyTexture(this->texture);
    SDL_Quit();
}

