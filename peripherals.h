#include <SDL2/SDL.h>

class peripherals {
    SDL_Window *window;
    SDL_Renderer *renderer;

    int width;
    int height;

public:
    void draw_display(bool **display, uint8_t display_width, uint8_t display_height);
    bool process_input(bool *keyboard, uint8_t num_keys);

    peripherals();        
    ~peripherals();
};
