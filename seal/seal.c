/*
 * Copyright (C) 2016 Tang Yiyang
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See BELOW for details.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "seal.h"

extern void (luaL_openlibs) (lua_State *L);
extern void luaopen_lua_extensions(lua_State *L);
extern void nuk_init(void* winctx);
extern void nanovg_init(int w, int h);

#define TRACE_BACK_FUNC_INDEX 1
#define UPDATE_FUNC_INDEX     2
#define DRAW_FUNC_INDEX       3
#define TOP_FUNC_INDEX        3

struct game* GAME = NULL;

int seal_call(lua_State *L, int n, int r)
{
    int err = lua_pcall(L, n, r, TRACE_BACK_FUNC_INDEX);
    switch(err) {
        case LUA_OK:
            break;
        case LUA_ERRRUN:
            LOGP_LUA("!LUA_ERRRUN : %s\n", lua_tostring(L,-1));
            s_assert(0);
            break;
        case LUA_ERRMEM:
            LOGP_LUA("!LUA_ERRMEM : %s\n", lua_tostring(L,-1));
             s_assert(0);
            break;
        case LUA_ERRERR:
            LOGP_LUA("!LUA_ERRERR : %s\n", lua_tostring(L,-1));
             s_assert(0);
            break;
        case LUA_ERRGCMM:
            LOGP_LUA("!LUA_ERRGCMM : %s\n", lua_tostring(L,-1));
             s_assert(0);
            break;
        default:
            LOGP_LUA("!Unknown Lua error: %d\n", err);
            s_assert(0);
            break;
    }
    return err;
}

void seal_call_func(void* object,
                    int (*stack_set_func)(lua_State*, void* ud),
                    void* ud,
                    bool cleanup)
{
    lua_handler_exe_func(GAME->lua_handler,
                            GAME->lstate, object, stack_set_func, ud, cleanup);
}

lua_State* seal_new_lua()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    return L;
}

int load_game_scripts(lua_State* L, const char* zipfile)
{
    size_t size = 0;
    unsigned char* filedata = fs_read(zipfile, &size, 0);
    if (!filedata) {
        LOGP("unable to read zipfile = %s\n", zipfile);
        return 1;
    }

    unzFile unzfile = unzOpenBuffer(filedata, size);
    if(!unzfile) {
        LOGP("open zip from buffer failed.\n");
        return 1;
    }

    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");

    char filename[1024] = "";
    int err = unzGoToFirstFile(unzfile);
    if (err) {
        LOGP("go to first file failed");
        return 1;
    }

    bool succeed = true;
    while (true) {
        unz_file_info info;
        err = unzGetCurrentFileInfo(unzfile, &info, filename,
                                    1024, NULL, 0, NULL, 0);
        if (err) {
            LOGP("get current file info failed," "filename = %s\n", filename);
            succeed = false;
            break;
        }

        unz_file_pos file_pos;
        err = unzGetFilePos(unzfile, &file_pos);
        err = unzGoToFilePos(unzfile, &file_pos);
        err = unzOpenCurrentFile(unzfile);
        unsigned long size = info.uncompressed_size;
        unsigned char* buffer = s_malloc(size);
        unsigned int readed = unzReadCurrentFile(unzfile,
                                                    buffer,
                                                    (unsigned int)size);
        if(readed != size) {
            succeed = false;
            LOGP("read zip file failed? error size,"
                            "required = %ld, readed = %d, filename = %s\n",
                            size, readed, filename);
            goto error;
        }
        err = luaL_loadbuffer(L, (const char*)buffer, size, filename);
        if(err) {
            LOGP("error loadbuffer, filename = %s\n", filename);
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

void seal_init_graphics(int w, int h)
{
    // baisc graphic modules
    GAME->texture_cache = texture_cache_new();
    GAME->sprite_frame_cache = sprite_frame_cache_new();
    GAME->bmfont_cache = bmfont_cache_new();
    GAME->global_camera = camera_new(w, h);
    GAME->render = render_new();
    GAME->lua_handler = lua_handler_new(GAME->lstate);
    GAME->scheduler = scheduler_new();
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
}

void seal_load_string(const char* script_data)
{
    if(luaL_dostring(GAME->lstate, script_data)) {
        LOGP("run start script Failed. %s\n", lua_tostring(GAME->lstate, -1));
        lua_pop(GAME->lstate, 1);
    }
}

void seal_load_file(const char* script_path)
{
#if defined PLAT_DESKTOP
    const char* buff = fs_reads(script_path);

    if (luaL_loadbuffer(GAME->lstate, buff, strlen(buff), script_path)) {
        LOGP("load start script Failed. %s \n", lua_tostring(GAME->lstate, -1));
        lua_pop(GAME->lstate, 1);
    }
    else {
        if (lua_pcall(GAME->lstate, 0, LUA_MULTRET, 0)) {
            LOGP("run start script Failed. %s \n", lua_tostring(GAME->lstate, -1));
            lua_pop(GAME->lstate, 1);
        }
    }

    seal_free(buff);

    //if(luaL_dofile(GAME->lstate, script_path)) {
    //    LOGP("run start script Failed. %s \n",
    //            lua_tostring(GAME->lstate, -1));
    //}

#elif defined PLAT_IOS
    char* script_file_data = fs_reads(script_path);
    seal_load_string(script_file_data);
    s_free(script_file_data);

#elif defined PLAT_ANDROID
    size_t buff_len = 0;
    const char* buff = fs_read(script_path, &buff_len, 0);
    if (luaL_loadbuffer(GAME->lstate, buff, buff_len, script_path)) {
        LOGP_LUA("load start script Failed. %s \n", lua_tostring(GAME->lstate, -1));
        lua_pop(GAME->lstate, 1);
    }
    else {
        if (lua_pcall(GAME->lstate, 0, LUA_MULTRET, TRACE_BACK_FUNC_INDEX)) {
            LOGP("run script[%s] Failed. %s \n", script_path, lua_tostring(GAME->lstate, -1));
            lua_pop(GAME->lstate, 1);
        }
    }

    seal_free(buff);

#endif

}

int get_traceStack (lua_State *L) {
    int level;
    lua_Debug ar;
    char buff[1024] = {0};
    char out[102400] = {0};

    for (level = 0; level < 20; ++level)
    {
        if (lua_getstack(L, level, &ar))
        {
            lua_getinfo(L, "Snl", &ar);
            sprintf(buff, "%02d) %s:%d:in function '%s' %d\n", level, ar.source, ar.currentline, ar.name, ar.linedefined);
            strcat(out, buff);
        }
    }
    lua_pushstring(L, out);
    return 1;
}

static int traceback (lua_State *L)
{
    const char *msg = lua_tostring(L, 1);
    if (msg) {
        luaL_traceback(L, L, msg, 0);
        const char* tm = lua_tostring(L, -1);
        LOGP_LUA("%s", tm);
    } else if (!lua_isnoneornil(L, 1)) {
        if (!luaL_callmeta(L, 1, "__tostring")) {
            lua_pushliteral(L, "(no error message)");
        }
    }
    return 1;
}

static int on_seal_game_start(lua_State* L, void* ud)
{
    lua_pushinteger(L, EVENT_GAME_START);
    return 1;
}

void seal_start_game()
{
    gettimeofday(&GAME->__last_update, NULL);

    // the bootloader
    seal_load_file("scripts/bootloader.lua");
    //seal_load_string("main()");


    lua_State *L = GAME->lstate;
    lua_settop(L, 0);
    assert(lua_gettop(L) == 0);
    lua_pushcfunction(L, traceback);

    //lua_getglobal(L, "__traceback__");

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

    float dt = ((now.tv_sec - GAME->__last_update.tv_sec) +
                   (now.tv_usec - GAME->__last_update.tv_usec))/1000000.0f;

    GAME->__last_update = now;

    if (dt < FLT_EPSILON) {
        return;
    }

    GAME->global_dt = dt;
    camera_update(GAME->global_camera);
    scheduler_update(GAME->scheduler, dt);

    lua_State* L = GAME->lstate;
    lua_pushvalue(L, UPDATE_FUNC_INDEX);
    lua_pushnumber(L, dt);
    seal_call(L, 1, 0);
    lua_settop(L, TOP_FUNC_INDEX);
}

void seal_event(int event_type,
                int (*stack_set_func)(lua_State*, void*),
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
    scheduler_free(GAME->scheduler);
    s_free(GAME);
}

int on_seal_key_receive(lua_State* L, void* ud)
{
    struct key_event* event = (struct key_event*)ud;
    lua_pushinteger(L, EVENT_KEY_RECEIVE);
    lua_pushinteger(L, event->key);
    lua_pushinteger(L, event->action);
    return 3;
}