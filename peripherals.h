#include <SDL2/SDL.h>

class peripherals {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    int display_width;
    int display_height;

public:
    void draw_display(void *const display, int pitch);
    bool process_input(bool *keyboard);

    peripherals(int window_width, int window_height, int display_width, int display_height);        
    ~peripherals();
};
