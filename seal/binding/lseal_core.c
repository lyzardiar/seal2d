#include "lauxlib.h"

#include "seal.h"

extern void stackDump (lua_State *L);

int lsealget_window(lua_State* L) {
//    lua_pushlightuserdata(L, seal_get_window());
    return 1;
}

int lsealget_render(lua_State* L) {
//    lua_pushlightuserdata(L, seal_get_render());
    return 1;
}

int lsealinject(lua_State* L) {
    if(!lua_istable(L, -1)) {
        fprintf(stderr, "seal.start require a table to start.");
        exit(1);
    }
    
    lua_getfield(L, -1, "update");
    lua_setfield(L, LUA_REGISTRYINDEX, GAME_UPDATE);
    
    lua_getfield(L, -1, "draw");
    lua_setfield(L, LUA_REGISTRYINDEX, GAME_DRAW);
    
    lua_getfield(L, -1, "pause");
    lua_setfield(L, LUA_REGISTRYINDEX, GAME_PAUSE);
    
    lua_getfield(L, -1, "resume");
    lua_setfield(L, LUA_REGISTRYINDEX, GAME_RESUME);
    
    lua_getfield(L, -1, "event");
    lua_setfield(L, LUA_REGISTRYINDEX, GAME_EVENT);
    
    lua_pop(L, -1);
    return 0;
}

int lsealget_platform(lua_State* L) {
    // TODO: write this function someday
    lua_pushstring(L, "mac");
    return 1;
}

int luaopen_seal_core(lua_State* L) {
    
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { "inject",     lsealinject},
        { "get_window", lsealget_window},
        { "get_render", lsealget_render},
        { "get_platform", lsealget_platform},
        { NULL, NULL },
    };
    
    luaL_newlib(L, lib);
    
    return 1;
}