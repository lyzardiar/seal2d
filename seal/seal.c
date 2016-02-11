#include "seal.h"
#include "lopen.h"

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
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "!LUA_ERRRUN : %s\n", lua_tostring(L,-1));
            SDL_assert(SDL_FALSE);
            break;
        case LUA_ERRMEM:
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "!LUA_ERRMEM : %s\n", lua_tostring(L,-1));
            SDL_assert(SDL_FALSE);
            break;
        case LUA_ERRERR:
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "!LUA_ERRERR : %s\n", lua_tostring(L,-1));
            SDL_assert(SDL_FALSE);
            break;
        case LUA_ERRGCMM:
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "!LUA_ERRGCMM : %s\n", lua_tostring(L,-1));
            SDL_assert(SDL_FALSE);
            break;
        default:
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "!Unknown Lua error: %d\n", err);
            SDL_assert(SDL_FALSE);
            break;
    }
    return err;
}

lua_State* seal_new_lua() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    return L;
}

void seal_init() {
    SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_TIMER);
    IMG_Init(IMG_INIT_PNG);

    GAME = (struct game*)SDL_malloc(sizeof(struct game));
    
    // lua modules
    lua_State* L = seal_new_lua();
    luaopen_lua_extensions(L);
    GAME->lstate = L;
    
    // load the game settings from config.lua
    seal_load_file("scripts/config.lua");
    
    lua_getglobal(L, "APP_NAME");
    lua_getglobal(L, "WINDOW_WIDTH");
    lua_getglobal(L, "WINDOW_HEIGHT");
    
    const char* app_name = lua_tostring(L, 1);
    GAME->window_width = lua_tonumber(L, 2);
    GAME->window_height = lua_tonumber(L, 3);
    
    // SDL core modules
    SDL_Window* window = SDL_CreateWindow(app_name, 0, 0,
                                          GAME->window_width, GAME->window_height, SDL_WINDOW_ALLOW_HIGHDPI);
    if(!window) {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "window craete failed.\n");
        exit(1);
    }
    GAME->window = window;
    
    // create the render
    SDL_Renderer* render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!render) {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "create gl render failed.\n");
        exit(1);
    }
    GAME->gl_render = render;
    
    // create the freetype lib
    if(TTF_Init() == -1) {
        SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "create ttf lib failed.\n");
        exit(1);
    }
    
    lua_pop(L, 3);
}

void seal_load_string(const char* script_data) {
    if(luaL_dostring(GAME->lstate, script_data)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "run start script Failed.\n");
        abort();
    }
}

void seal_load_file(const char* script_path) {
    if(luaL_dofile(GAME->lstate, script_path)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "run start script Failed.\n");
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

void seal_start_game() {
    lua_State *L = GAME->lstate;
    SDL_assert(lua_gettop(L) == 0);
    lua_pushcfunction(L, traceback);
    lua_getfield(L,LUA_REGISTRYINDEX, GAME_UPDATE);
    lua_getfield(L,LUA_REGISTRYINDEX, GAME_DRAW);
    lua_getfield(L,LUA_REGISTRYINDEX, GAME_PAUSE);
    lua_getfield(L,LUA_REGISTRYINDEX, GAME_RESUME);
    lua_getfield(L,LUA_REGISTRYINDEX, GAME_EVENT);
}

void seal_set_window(SDL_Window* window) {
    GAME->window = window;
}

void seal_event(const SDL_Event* event) {
    lua_State* L = GAME->lstate;
    lua_getfield(L, LUA_REGISTRYINDEX, GAME_EVENT);
    
    lua_newtable(L);
    
    switch (event->type) {
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEWHEEL:
            setfiled_i(L, "type", event->motion.type);
            setfiled_i(L, "state", event->motion.state);
            setfiled_i(L, "x", event->motion.x);
            setfiled_i(L, "y", event->motion.y);
            setfiled_i(L, "xrel", event->motion.xrel);
            setfiled_i(L, "yrel", event->motion.yrel);
            seal_call(L, 1, 0);
            break;
            
        case SDL_FINGERDOWN:
        case SDL_FINGERUP:
        case SDL_FINGERMOTION:
            setfiled_i(L, "type", event->tfinger.type);
            setfiled_i(L, "timestamp", event->tfinger.timestamp);
            setfiled_i(L, "touchId", event->tfinger.touchId);
            setfiled_i(L, "fingerId", event->tfinger.fingerId);
            setfiled_f(L, "x", event->tfinger.x * GAME->window_width);
            setfiled_f(L, "y", event->tfinger.y * GAME->window_height);
            setfiled_f(L, "dx", event->tfinger.dx * GAME->window_width);
            setfiled_f(L, "dy", event->tfinger.dy * GAME->window_height);
            setfiled_f(L, "pressure", event->tfinger.pressure);
            seal_call(L, 1, 0);
            break;
        case SDL_DOLLARGESTURE:
        case SDL_DOLLARRECORD:
      
            break;
        case SDL_MULTIGESTURE:
            setfiled_i(L, "type", event->mgesture.type);
            setfiled_i(L, "timestamp", event->mgesture.timestamp);
            setfiled_i(L, "touchId", event->mgesture.touchId);
            setfiled_f(L, "dTheta", event->mgesture.dTheta);
            setfiled_f(L, "dDist", event->mgesture.dDist);
            setfiled_f(L, "x", event->mgesture.x);
            setfiled_f(L, "y", event->mgesture.y);
            setfiled_i(L, "numFingers", event->mgesture.numFingers);
            setfiled_i(L, "padding", event->mgesture.padding);
            seal_call(L, 1, 0);

            break;
        default:
            break;
    }
    lua_settop(GAME->lstate, TOP_FUNC_INDEX);
}

void seal_update(float dt) {
    lua_State* L = GAME->lstate;

    lua_pushvalue(L, UPDATE_FUNC_INDEX);
    lua_pushnumber(L, dt);

    seal_call(L, 1, 0);
    lua_settop(L, TOP_FUNC_INDEX);
}

void seal_draw() {
    lua_State* L = GAME->lstate;
    
    lua_pushvalue(L, DRAW_FUNC_INDEX);
    seal_call(L, 0, 0);
    lua_settop(L, TOP_FUNC_INDEX);
}

void seal_destroy() {
// TODO: clean up every thing here.
    
//    lua_close(GAME->lstate);
//    SDL_free(GAME);
}

SDL_Window* seal_get_window() {
    SDL_assert(GAME);
    return GAME->window;
}

SDL_Renderer* seal_get_render() {
    return GAME->gl_render;
}