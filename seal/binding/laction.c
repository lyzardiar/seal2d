#include "lopen.h"

#include "lua.h"
#include "lauxlib.h"

#include "seal.h"
#include "base/array.h"

#include "action.h"
#include "lua_handler.h"

EXTERN_GAME;

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

int laction_sequence(lua_State* L)
{
    s_assert(lua_istable(L, -1));

    lua_len(L, -1);
    int cnt = lua_tointeger(L, -1);
    lua_pop(L, 1);
    struct array* actions = array_new(cnt);
    for (int i = 0; i < cnt; ++i) {
        lua_rawgeti(L, -1, i+1);
        array_push_back(actions, lua_touserdata(L, -1));
        lua_pop(L, 1);
    }

    lua_pushlightuserdata(L, sequence(actions));
    return 1;
}

int laction_call_func(lua_State* L)
{
    struct action* call = call_lua_func();
    LUA_FUNCTION_HANDLER handler = lua_handler_new_func(GAME->lua_handler,
                         L,
                         call,
                         1);
    ((struct action_call_lua_func*)call->__child)->lua_func = handler;
    lua_pushlightuserdata(L, call);
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
        { "sequence", laction_sequence },
        { "call_func", laction_call_func },
        { NULL, NULL },
    };

    luaL_newlib(L, lib);

    return 1;
}
