#include "lopen.h"

#include "lua.h"
#include "lauxlib.h"

#include "seal.h"

#include "action.h"

int laction_move_to(lua_State* L)
{
    lua_Number duration = luaL_checknumber(L, 1);
    lua_Number x = luaL_checknumber(L, 2);
    lua_Number y = luaL_checknumber(L, 3);

    lua_pushlightuserdata(L, move_to(duration, x, y));
    return 1;
}

int laction_ease_in(lua_State* L)
{
    struct action* action = lua_touserdata(L, 1);
    lua_Number rate = luaL_checknumber(L, 2);

    lua_pushlightuserdata(L, ease_in(action ,rate));
    return 1;
}

int luaopen_seal_action(lua_State* L)
{
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { "move_to", laction_move_to },
        { "ease_in", laction_ease_in },
        { NULL, NULL },
    };

    luaL_newlib(L, lib);

    return 1;
}
