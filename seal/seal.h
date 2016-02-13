#ifndef __seal__seal__
#define __seal__seal__

#define GL3_PROTOTYPES 1
#include <OpenGL/gl3.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include "shader.h"
#include "vertex.h"

struct game {
    lua_State* lstate;
    SDL_Window* window;
    SDL_Renderer* gl_render;
    
    Uint32 window_width;
    Uint32 window_height;
};

#define GAME_TRACE  "GAME_TRACE"
#define GAME_UPDATE "GAME_UPDATE"
#define GAME_DRAW   "GAME_DRAW"
#define GAME_PAUSE  "GAME_PAUSE"
#define GAME_RESUME "GAME_RESUME"
#define GAME_EVENT  "GAME_EVENT"

// seal system
void seal_init();
void seal_load_string(const char* script_data);
void seal_load_file(const char* script_path);
void seal_start_game();
void seal_set_window(SDL_Window* window);
void seal_event(const SDL_Event* event);
void seal_update(float dt);
void seal_draw();
void seal_destroy();

SDL_Window* seal_get_window();
SDL_Renderer* seal_get_render();

#endif