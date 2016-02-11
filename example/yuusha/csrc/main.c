#include "seal.h"

int main(int argc, char *argv[]) {
    seal_init();
    seal_load_file("scripts/startup.lua");
    seal_start_game();

    SDL_bool running = SDL_TRUE;
    SDL_Event event;
    
    float interval = (1 / 60.0f) * 1000;
    
    Uint32 last = SDL_GetTicks();
    Uint32 now = 0;
    Sint32 dt = 0;
    while (running) {
        if (dt > 0) {
            seal_update(dt);
            seal_draw();
        }
    
        now = SDL_GetTicks();
        
        dt = now - last;
        dt = dt > 0 ? dt : 0;

        last = now;
        if (dt > 0 && dt < interval) {
            SDL_Delay(1);
        }
        
        while(SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = SDL_FALSE;
            }
            seal_event(&event);
        }
    }
    seal_destroy();
    return 0;
}
