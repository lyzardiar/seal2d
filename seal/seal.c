#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <float.h>
#include <stddef.h>

#include "lualib.h"
#include "lauxlib.h"

#include "seal.h"
#include "geo.h"
#include "window.h"
#include "shader.h"
#include "sprite.h"
#include "camera.h"
#include "render.h"

#include "math/matrix.h"

#include "util.h"
#include "lopen.h"
#include "ttf_font.h"
#include "event.h"
#include "platform/fs.h"
#include "platform/platform.h"
#include "unzip.h"

#define NK_INCLUDE_FIXED_TYPES
#include "nuklear/nuklear.h"

#ifdef PLAT_DESKTOP
#include "nuklear/nuklear_glfw_gl3.h"
#endif

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

extern void luaopen_lua_extensions(lua_State *L);

#define TRACE_BACK_FUNC_INDEX 1
#define UPDATE_FUNC_INDEX     2
#define DRAW_FUNC_INDEX       3
#define TOP_FUNC_INDEX        3

struct game* GAME = NULL;

int seal_call(lua_State *L, int n, int r) {
    int err = lua_pcall(L, n, r, TRACE_BACK_FUNC_INDEX);
    switch(err) {
        case LUA_OK:
            break;
        case LUA_ERRRUN:
            fprintf(stderr, "!LUA_ERRRUN : %s\n", lua_tostring(L,-1));
            s_assert(0);
            break;
        case LUA_ERRMEM:
            fprintf(stderr, "!LUA_ERRMEM : %s\n", lua_tostring(L,-1));
             s_assert(0);
            break;
        case LUA_ERRERR:
            fprintf(stderr, "!LUA_ERRERR : %s\n", lua_tostring(L,-1));
             s_assert(0);
            break;
        case LUA_ERRGCMM:
            fprintf(stderr, "!LUA_ERRGCMM : %s\n", lua_tostring(L,-1));
             s_assert(0);
            break;
        default:
            fprintf(stderr, "!Unknown Lua error: %d\n", err);
            s_assert(0);
            break;
    }
    return err;
}

lua_State* seal_new_lua() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    return L;
}

struct ttf_font* font = NULL;

int load_game_scripts(lua_State* L, const char* zipfile) {
    size_t size = 0;
    unsigned char* filedata = s_read(zipfile, &size, 0);
    if (!filedata) {
        fprintf(stderr, "unable to read zipfile = %s\n", zipfile);
        return 1;
    }
    
    unzFile unzfile = unzOpenBuffer(filedata, size);
    if(!unzfile) {
        fprintf(stderr, "open zip from buffer failed.\n");
        return 1;
    }
    
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    
    char filename[1024] = "";
    int err = unzGoToFirstFile(unzfile);
    if (err) {
        fprintf(stderr, "go to first file failed");
        return 1;
    }
    
    bool succeed = true;
    while (true) {
        unz_file_info info;
        err = unzGetCurrentFileInfo(unzfile, &info, filename, 1024, NULL, 0, NULL, 0);
        if (err) {
            fprintf(stderr, "get current file info failed, filename = %s\n", filename);
            succeed = false;
            break;
        }
        
        unz_file_pos file_pos;
        err = unzGetFilePos(unzfile, &file_pos);
        err = unzGoToFilePos(unzfile, &file_pos);
        err = unzOpenCurrentFile(unzfile);
        unsigned long size = info.uncompressed_size;
        unsigned char* buffer = s_malloc(size);
        unsigned int readed = unzReadCurrentFile(unzfile, buffer, (unsigned int)size);
        if(readed != size) {
            succeed = false;
            fprintf(stderr, "read zip file failed? error size, required = %ld, readed = %d, filename = %s\n",
                        size, readed, filename);
            goto error;
        }
        err = luaL_loadbuffer(L, (const char*)buffer, size, filename);
        if(err) {
            fprintf(stderr, "error loadL_loadbuffer, filename = %s\n", filename);
            goto error;
        }
        lua_setfield(L, -2, filename);
        
        if(unzGoToNextFile(unzfile) == UNZ_END_OF_LIST_OF_FILE) {
            succeed = true;
            break;
        }
        unzCloseCurrentFile(unzfile);
        s_free(buffer);
        continue;
    error:
        succeed = false;
        unzCloseCurrentFile(unzfile);
        s_free(buffer);
        break;
    }
    
    lua_pop(L, -1);
    stackDump(L);
    return succeed ? 1 : 0;
}

void seal_reload_scripts() {
    load_game_scripts(GAME->lstate, "res/code.zip");
}

struct game* seal_load_game_config() {
    GAME = (struct game*)s_malloc(sizeof(struct game));
    // lua modules
    lua_State* L = seal_new_lua();
    luaopen_lua_extensions(L);
    GAME->lstate = L;
    
    // load the game settings from config.lua
    seal_load_file("scripts/config.lua");
    lua_getglobal(L, "APP_NAME");
    lua_getglobal(L, "WINDOW_WIDTH");
    lua_getglobal(L, "WINDOW_HEIGHT");
    lua_getglobal(L, "NK_GUI_FONT_PATH");
    lua_getglobal(L, "NK_GUI_FONT_SIZE");
    
    struct game_config* config = &GAME->config;
    config->app_name = lua_tostring(L, 1);
    config->window_width = lua_tonumber(L, 2);
    config->window_height = lua_tonumber(L, 3);
    config->nk_gui_font_path = lua_tostring(L, 4);
    config->nk_gui_font_size = lua_tonumber(L, 5);
    
    lua_pop(L, 5);
    
    GAME->window = win_alloc();

    return GAME;
}

void seal_init_graphics() {
    
    // baisc graphic modules
    GAME->texture_cache = texture_cache_new();
    GAME->sprite_frame_cache = sprite_frame_cache_new();
    GAME->global_camera = camera_new(GAME->config.window_height, GAME->config.window_height);
    GAME->render = render_new();
    
    sprite_init_render(GAME->render);
    
    // init the font
    ttf_init_module();
    font = ttf_font_new("res/fonts/marker_felt.ttf", 100);  //TODO: load this in Lua.
    GAME->font = font;
    
    // the bootloader
    seal_load_file("scripts/bootloader.lua");
    seal_load_string("main()");
}

void seal_load_string(const char* script_data) {
    if(luaL_dostring(GAME->lstate, script_data)) {
        fprintf(stderr, "run start script Failed. %s\n", lua_tostring(GAME->lstate, -1));
        abort();
    }
}

void seal_load_file(const char* script_path) {
    if(luaL_dofile(GAME->lstate, script_path)) {
        fprintf(stderr, "run start script Failed.\n");
        abort();
    }
}

static int traceback (lua_State *L) {
    const char *msg = lua_tostring(L, 1);
    if (msg)
        luaL_traceback(L, L, msg, 1);
    else if (!lua_isnoneornil(L, 1)) {
        if (!luaL_callmeta(L, 1, "__tostring"))
            lua_pushliteral(L, "(no error message)");
    }
    return 1;
}

static struct timeval _lastUpdate;

void seal_start_game() {
    gettimeofday(&_lastUpdate, NULL);
    
    lua_State *L = GAME->lstate;
    assert(lua_gettop(L) == 0);
    lua_pushcfunction(L, traceback);

    lua_getfield(L,LUA_REGISTRYINDEX, GAME_INIT);
    seal_call(L, 0, 1);

    struct sprite* root = lua_touserdata(L, -1);
    GAME->root = root;
    lua_pop(L, 1);

    lua_getfield(L,LUA_REGISTRYINDEX, GAME_UPDATE);
    lua_getfield(L,LUA_REGISTRYINDEX, GAME_DRAW);
    lua_getfield(L,LUA_REGISTRYINDEX, GAME_PAUSE);
    lua_getfield(L,LUA_REGISTRYINDEX, GAME_RESUME);
    lua_getfield(L,LUA_REGISTRYINDEX, GAME_EVENT);
    
    camera_pos(GAME->global_camera,
               GAME->config.window_width/2,
               GAME->config.window_height/2);
}

void seal_update() {
    struct timeval now;
    gettimeofday(&now, NULL);
    
    // delta只计算draw的时间
    float dt = ((now.tv_sec - _lastUpdate.tv_sec) +
                   (now.tv_usec - _lastUpdate.tv_usec))/1000000.0f;

    _lastUpdate = now;

    if (dt < FLT_EPSILON) {
        return;
    }
    
    GAME->global_dt = dt;
    camera_update(GAME->global_camera);
    
    lua_State* L = GAME->lstate;

    lua_pushvalue(L, UPDATE_FUNC_INDEX);
    lua_pushnumber(L, dt);

    seal_call(L, 1, 0);
    lua_settop(L, TOP_FUNC_INDEX);
}

void seal_event(struct event* e) {
    lua_State* L = GAME->lstate;
    lua_getfield(GAME->lstate, LUA_REGISTRYINDEX, GAME_EVENT);
    switch (e->type) {
        case TOUCH_BEGIN:
        case TOUCH_MOVE:
        case TOUCH_END:
        case TOUCH_CANCEL:
            lua_newtable(L);
            lua_pushinteger(L, e->type);
            lua_setfield(L, -2, "type");
            lua_pushinteger(L, e->x);
            lua_setfield(L, -2, "x");
            lua_pushinteger(L, e->y);
            lua_setfield(L, -2, "y");
            lua_call(L, 1, 0);
            lua_settop(L, TOP_FUNC_INDEX);
            break;
            
        default:
            break;
    }
}

void seal_draw() {
    struct render* R = GAME->render;
    render_clear(R, MAKE_COLOR(255, 255, 255, 255));
    
    sprite_visit(GAME->root, GAME->global_dt);
    
    render_commit(R);
    
    CHECK_GL_ERROR
}

void seal_destroy() {
    
    lua_close(GAME->lstate);
    
    texture_cache_free(GAME->texture_cache);
    sprite_frame_cache_free(GAME->sprite_frame_cache);
    win_free(GAME->window);
    
// memory is managed by Lua, don't need to free
//    sprite_free(GAME->root);
    s_free(GAME);
}

void nk_draw(void* win_ctx) {
    struct nk_context* ctx = GAME->nk_gui_ctx;
    nk_glfw3_new_frame();
    
    /* GUI */
    struct nk_panel layout;
    if (nk_begin(ctx, &layout, "Demo", nk_rect(50, 50, 230, 250),
                 NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                 NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {
        enum {EASY, HARD};
        static int op = EASY;
        static int property = 20;
        nk_layout_row_static(ctx, 30, 80, 1);
        if (nk_button_label(ctx, "button", NK_BUTTON_DEFAULT))
            fprintf(stdout, "button pressed\n");
        
        nk_layout_row_dynamic(ctx, 30, 2); 
        if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
        if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;
        
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);
    }
    nk_end(ctx);
    
    nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}