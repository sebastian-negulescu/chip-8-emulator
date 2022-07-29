#include <SDL2/SDL.h>

class peripherals {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;

    uint32_t *display;
    int display_width;
    int display_height;
    int pitch;

public:
    void draw_display(bool **display, int display_width, int display_height);
    bool process_input(bool *keyboard);

    peripherals(int window_width, int window_height, int display_width, int display_height);        
    ~peripherals();
};
