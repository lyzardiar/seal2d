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

    struct action* move = move_to(duration, x, y);
    lua_pushlightuserdata(L, move);
    return 1;
}

int luaopen_seal_action(lua_State* L)
{
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { "move_to", laction_move_to },
        { NULL, NULL },
    };

    luaL_newlib(L, lib);

    return 1;
}
