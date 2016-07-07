#ifndef __seal__seal__
#define __seal__seal__
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lua.h"

#include "memory.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION

#ifdef PLAT_DESKTOP
    #define NK_GLFW_GL3_IMPLEMENTATION
    #include "nuklear/nuklear_glfw_gl3.h"
    #define NK_INCLUDE_FIXED_TYPES
    #include "nuklear/nuklear.h"
#endif

struct camera;
struct sprite_batch;
struct sprite_frame_cache;
struct texture_cache;
struct ttf_font;
struct event;
struct window;
struct render;
struct nk_context;
struct touch_event;
struct lua_handler;

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
    
    // core render context
    float global_dt;
    struct camera* global_camera;
    struct sprite_batch* batch;
    struct texture_cache* texture_cache;
    struct ttf_font* font;
    struct window* window;
    struct sprite* root;             // the root node of the world
    struct shader* shader;
    struct render* render;
    struct lua_handler* lua_handler;
    struct sprite_frame_cache* sprite_frame_cache;
    
    // third party lib contexts
    struct nk_context* nk_gui_ctx;   // global context of the nuklear lib

    int game_state;
};

#define EXTERN_GAME extern struct game* GAME;

#define GAME_TRACE       "GAME_TRACE"
#define GAME_INIT        "GAME_INIT"
#define GAME_UPDATE      "GAME_UPDATE"
#define GAME_DRAW        "GAME_DRAW"
#define GAME_PAUSE       "GAME_PAUSE"
#define GAME_RESUME      "GAME_RESUME"
#define GAME_EVENT       "GAME_EVENT"
#define GAME_HANDLERS    "GAME_HANDLERS"

// seal system

struct game* seal_load_game_config();
void seal_init_graphics();
void seal_load_string(const char* script_data);
void seal_load_file(const char* script_path);
void seal_start_game();
void seal_main_loop();
void seal_update();
void seal_draw();
void seal_destroy();
int  seal_call(lua_State *L, int n, int r);
void seal_touch_event(struct touch_event* e);

void seal_reload_scripts();

#endif