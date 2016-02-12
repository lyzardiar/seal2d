#include "seal.h"

int main(int argc, char *argv[]) {
    seal_init();
    seal_load_file("scripts/startup.lua");
    seal_start_game();
    SDL_GL_SetSwapInterval(1);
    SDL_bool running = SDL_TRUE;
    SDL_Event event;
    
    Sint32 interval = (1 / 60) * 1000;
    
    Uint32 last = SDL_GetTicks();
    Uint32 now = 0;
    Sint32 dt = 16;
    while (running) {
        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_QUIT:
                    running = SDL_FALSE;
                    break;
                default:
                    break;
            }
        }
        
        now = SDL_GetTicks();
        
        dt = now - last;
        dt = dt > 0 ? dt : 0;
        last = now;
        if (dt > 0) {
            seal_update(dt);
            seal_draw();
        }
        
        
        if (dt > 0 && dt < interval) {
            SDL_Delay(interval - dt);
        }
    }
    seal_destroy();
    return 0;
}
