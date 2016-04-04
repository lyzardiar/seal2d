#ifndef __seal__seal__
#define __seal__seal__
#include <stdio.h>
#include <stdlib.h>

#include "lua.h"

#include "memory.h"

struct camera;
struct sprite_batch;
struct ttf_font;
struct event;
struct window;

struct game {
    lua_State* lstate;
    int window_width;
    int window_height;
    const char* app_name;
    
    struct camera* global_camera;
    struct sprite_batch* batch;
    struct ttf_font* font;
    struct window* window;
    struct sprite* root;   // the root node of the world
};

#define EXTERN_GAME extern struct game* GAME;

#define GAME_TRACE  "GAME_TRACE"
#define GAME_UPDATE "GAME_UPDATE"
#define GAME_DRAW   "GAME_DRAW"
#define GAME_PAUSE  "GAME_PAUSE"
#define GAME_RESUME "GAME_RESUME"
#define GAME_EVENT  "GAME_EVENT"

// seal system

struct game* seal_load_game_config();
void seal_init_graphics();
void seal_load_string(const char* script_data);
void seal_load_file(const char* script_path);
void seal_start_game();
void seal_main_loop();
void seal_update(float dt);
void seal_draw();
void seal_destroy();
void seal_event(struct event* e);

#endif