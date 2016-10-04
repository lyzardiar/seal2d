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

#include "math/matrix.h"
#include "seal.h"
#include "geo.h"
#include "window.h"
#include "shader.h"
#include "sprite.h"
#include "bmfont.h"
#include "camera.h"
#include "render.h"
#include "lua_handler.h"

#include "util.h"
#include "lopen.h"
#include "ttf_font.h"
#include "event.h"
#include "platform/fs.h"
#include "unzip.h"

extern void luaopen_lua_extensions(lua_State *L);
extern void nuk_init(void* winctx);
extern void nanovg_init(int w, int h);

#define TRACE_BACK_FUNC_INDEX 1
#define UPDATE_FUNC_INDEX     2
#define DRAW_FUNC_INDEX       3
#define TOP_FUNC_INDEX        3

#define EVENT_GAME_START (0)
#define EVENT_GAME_END   (1)
struct game* GAME = NULL;

int seal_call(lua_State *L, int n, int r)
{
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

lua_State* seal_new_lua()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    return L;
}

struct ttf_font* font = NULL;

int load_game_scripts(lua_State* L, const char* zipfile)
{
    size_t size = 0;
    unsigned char* filedata = fs_read(zipfile, &size, 0);
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
    return succeed ? 1 : 0;
}

void seal_reload_scripts()
{
    load_game_scripts(GAME->lstate, "res/code.zip");
}

struct game* seal_load_game_config()
{
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

#ifdef PLAT_DESKTOP
    GAME->window = win_alloc();
#endif

    return GAME;
}

void seal_init_graphics()
{
    // baisc graphic modules
    GAME->texture_cache = texture_cache_new();
    GAME->sprite_frame_cache = sprite_frame_cache_new();
    GAME->bmfont_cache = bmfont_cache_new();
    GAME->global_camera = camera_new(GAME->config.window_height, GAME->config.window_height);
    GAME->render = render_new();
    GAME->lua_handler = lua_handler_new(GAME->lstate);
    sprite_init_render(GAME->render);

#ifdef PLAT_DESKTOP
    nuk_init(GAME->window->ctx);
    nanovg_init(GAME->config.window_width, GAME->config.window_height);
#endif
    // init the font
    // TODO: implement this later
//    ttf_init_module();
//    font = ttf_font_new("res/fonts/SourceCodePro-Regular.ttf", 32);  //TODO: load this in Lua.
//    GAME->font = font;

    // the bootloader
    seal_load_file("scripts/bootloader.lua");
    seal_load_string("main()");
}

void seal_load_string(const char* script_data)
{
    if(luaL_dostring(GAME->lstate, script_data)) {
        fprintf(stderr, "run start script Failed. %s\n", lua_tostring(GAME->lstate, -1));
        abort();
    }
}

void seal_load_file(const char* script_path)
{
#ifdef PLAT_DESKTOP
    if(luaL_dofile(GAME->lstate, script_path)) {
        fprintf(stderr, "run start script Failed. %s \n", lua_tostring(GAME->lstate, -1));
        abort();
    }
#endif

#ifdef PLAT_IOS
    char* script_file_data = fs_reads(script_path);
    seal_load_string(script_file_data);
    s_free(script_file_data);
#endif

}

static int traceback (lua_State *L)
{
    const char *msg = lua_tostring(L, 1);
    if (msg) {
        luaL_traceback(L, L, msg, 1);
    } else if (!lua_isnoneornil(L, 1)) {
        if (!luaL_callmeta(L, 1, "__tostring")) {
            lua_pushliteral(L, "(no error message)");
        }
    }
    return 1;
}

static struct timeval _lastUpdate;

static int on_seal_game_start(lua_State* L, void* ud)
{
    lua_pushinteger(L, EVENT_GAME_START);
    return 1;
}

void seal_start_game()
{
    gettimeofday(&_lastUpdate, NULL);

    lua_State *L = GAME->lstate;
    assert(lua_gettop(L) == 0);
    lua_pushcfunction(L, traceback);

    lua_getfield(L, LUA_REGISTRYINDEX, GAME_INIT);
    seal_call(L, 0, 1);

    struct sprite* root = lua_touserdata(L, -1);
    GAME->root = root;
    lua_pop(L, 1);

    lua_getfield(L, LUA_REGISTRYINDEX, GAME_UPDATE);
    lua_getfield(L, LUA_REGISTRYINDEX, GAME_DRAW);
    lua_getfield(L, LUA_REGISTRYINDEX, GAME_PAUSE);
    lua_getfield(L, LUA_REGISTRYINDEX, GAME_RESUME);
    lua_getfield(L, LUA_REGISTRYINDEX, GAME_EVENT);

    camera_pos(GAME->global_camera,
               GAME->config.window_width/2,
               GAME->config.window_height/2);

    seal_event(EVENT_GAME_START, on_seal_game_start, NULL);
}

void seal_update()
{
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

void seal_event(int event_type,
                int (*stack_set_func)(lua_State*, void* ud),
                void* ud)
{
    lua_State* L = GAME->lstate;
    lua_getfield(L, LUA_REGISTRYINDEX, GAME_EVENT);

    // push your event value here
    int n = 0;
    if (stack_set_func) {
        n = stack_set_func(L, ud);
    }

    seal_call(L, n, 0);
    lua_settop(L, TOP_FUNC_INDEX);
}

void seal_touch_event(struct touch_event* touch_event)
{
    sprite_touch(GAME->root, touch_event);
}


void seal_draw()
{
    struct render* R = GAME->render;
    render_clear(R, C4B_COLOR(255, 255, 255, 255));

    sprite_visit(GAME->root, GAME->global_dt);

    render_flush(R);

    // call the injected draw function in Lua Layer.
    lua_State* L = GAME->lstate;
    lua_pushvalue(L, DRAW_FUNC_INDEX);
    seal_call(L, 0, 0);
    lua_settop(L, TOP_FUNC_INDEX);
    
    CHECK_GL_ERROR
}

static int on_seal_game_end(lua_State* L, void* ud)
{
    lua_pushinteger(L, EVENT_GAME_END);
    return 1;
}

void seal_destroy()
{
    seal_event(EVENT_GAME_END, on_seal_game_end, NULL);

    lua_close(GAME->lstate);
    texture_cache_free(GAME->texture_cache);
    sprite_frame_cache_free(GAME->sprite_frame_cache);

#ifdef PLAT_DESKTOP
    win_free(GAME->window);
#endif

    sprite_free(GAME->root);
    s_free(GAME);
}