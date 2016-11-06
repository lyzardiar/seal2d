#include "../seal.h"

extern void stackDump (lua_State *L);

int lsealinject(lua_State* L)
{
    if(!lua_istable(L, -1)) {
        LOGP_LUA("seal.start require a table to start.");
        exit(1);
    }

    lua_getfield(L, -1, "init");
    lua_setfield(L, LUA_REGISTRYINDEX, GAME_INIT);

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

int lseal_reload_script(lua_State* L)
{
    seal_reload_scripts();
    return 0;
}

int luaopen_seal_core(lua_State* L)
{
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { "inject",     lsealinject},
        { "reload_script", lseal_reload_script},
        { NULL, NULL },
    };

    luaL_newlib(L, lib);

    return 1;
}