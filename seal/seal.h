#ifndef __seal__seal__
#define __seal__seal__
#include <stdio.h>
#include <stdlib.h>

#include "lua.h"


#include "memory.h"


struct NVGcontext;

struct camera;
struct sprite_batch;
struct texture_cache;
struct ttf_font;
struct event;
struct window;
struct nk_context;

enum GAME_STATE {
    GAME_STATE_INIT = 0,
    GAME_STATE_RUNNING,
    GAME_STATE_PAUSED,
    GAME_STATE_STOPPED,
};

struct game_config {
    int window_width;
    int window_height;
    const char* app_name;
    
    const char* nk_gui_font_path;
    int nk_gui_font_size;
};

struct game {
    lua_State* lstate;
    
    struct game_config config;
    
    struct camera* global_camera;
    struct sprite_batch* batch;
    struct texture_cache* texture_cache;
    struct ttf_font* font;
    struct window* window;
    struct sprite* root;   // the root node of the world
    struct nk_context* nk_gui_ctx; // global context of the nuklear lib
    
    int game_state;
};

#define EXTERN_GAME extern struct game* GAME;

#define GAME_TRACE  "GAME_TRACE"
#define GAME_INIT   "GAME_INIT"
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
void seal_update(struct NVGcontext* vg);
void seal_draw(void* win_ctx);
void seal_destroy();
void seal_event(struct event* e);

void seal_reload_scripts();

void nk_draw(void* win_ctx);

#endif