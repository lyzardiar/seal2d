#ifndef __seal__seal__
#define __seal__seal__

#include "lua.h"

#include "memory.h"

struct camera;
struct sprite_batch;
struct ttf_font;

struct game {
    lua_State* lstate;
    int window_width;
    int window_height;
    
    struct camera* global_camera;
    struct sprite_batch* batch;
    struct ttf_font* font;
};

#define EXTERN_GAME extern struct game* GAME;

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
void seal_main_loop();
void seal_update(float dt);
void seal_draw();
void seal_destroy();

#endif