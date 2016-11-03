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



#include "../seal.h"

EXTERN_GAME;

int laction_move_to(lua_State* L)
{
    lua_Number duration = luaL_checknumber(L, 1);
    lua_Number x = luaL_checknumber(L, 2);
    lua_Number y = luaL_checknumber(L, 3);

    lua_pushlightuserdata(L, move_to(duration, x, y));
    return 1;
}

int laction_scale_to(lua_State* L)
{
    lua_Number duration = luaL_checknumber(L, 1);
    lua_Number x = luaL_checknumber(L, 2);
    lua_Number y = luaL_checknumber(L, 3);

    lua_pushlightuserdata(L, scale_to(duration, x, y));
    return 1;
}

int laction_fade_to(lua_State* L)
{
    lua_Number duration = luaL_checknumber(L, 1);
    lua_Integer to = luaL_checkinteger(L, 2);

    lua_pushlightuserdata(L, fade_to(duration, to));
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
    LUA_FUNCTION_HANDLER handler = lua_handler_new_func(
                                                        GAME->lua_handler,
                                                        L,
                                                        call,
                                                        1);
    call->__internal.action_call.lua_func = handler;
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
        { "scale_to", laction_scale_to },
        { "fade_to", laction_fade_to },
        { "ease_in", laction_ease_in },
        { "sequence", laction_sequence },
        { "call_func", laction_call_func },
        { NULL, NULL },
    };

    luaL_newlib(L, lib);

    return 1;
}
