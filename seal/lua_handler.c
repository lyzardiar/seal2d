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


static int hash_str(void* key) {
    return hashmapHash(key, sizeof(void*));
}

static bool hash_equal(void* a, void* b) {
    return a == b;
}

struct lua_handler* lua_handler_new(lua_State* L) {
    struct lua_handler* handler = STRUCT_NEW(lua_handler);
    handler->__handlers = hashmapCreate(128, hash_str, hash_equal);
    handler->__func_index = 1;

    lua_newtable(L);
    lua_setfield(L, LUA_REGISTRYINDEX, LUA_FUNCTION_HANDLER_KEY);
    return handler;
}

void lua_handler_free(struct lua_handler* self) {
    hashmapFree(self->__handlers);
    s_free(self);;
}

void lua_handler_set(struct lua_handler* self,
                     void* object,
                     LUA_FUNCTION_HANDLER lua_function) {
    s_assert(object);
    hashmapPut(self->__handlers, object, (void*)lua_function);
}

void lua_handler_clean(struct lua_handler* self,
                       lua_State* L,
                       void* object) {
    s_assert(object);

    unsigned long index = (unsigned long)hashmapGet(self->__handlers, object);
    hashmapRemove(self->__handlers, object);
    lua_getfield(L, LUA_REGISTRYINDEX, LUA_FUNCTION_HANDLER_KEY);
    lua_pushinteger(L, index);
    lua_pushnil(L);
    lua_rawset(L, -3);
    lua_pop(L, 1);
}

unsigned int lua_handler_new_func(struct lua_handler* self,
                                  lua_State* L,
                                  void* object,
                                  int func_loc) {
    if (lua_isfunction(L, func_loc)) {
        unsigned int index = self->__func_index++;

        // set the function to LUA_REGISTRY
        lua_getfield(L, LUA_REGISTRYINDEX, LUA_FUNCTION_HANDLER_KEY);
        lua_pushinteger(L, index);
        lua_pushvalue(L, func_loc);
        lua_rawset(L, -3);
        lua_pop(L, 1);

        // save the index to the object, so that the object would be able to obtain the function.
        lua_handler_set(self, object, index);
        return index;
    }
    return 0;
}

void lua_handler_exe_func(struct lua_handler* self,
                          lua_State* L,
                          void* object,
                          int (*stack_set_func)(lua_State*, void* ud),
                          void* ud,
                          bool cleanup) {
    s_assert(object);
    int n = 0;
    unsigned int index = (unsigned int)hashmapGet(self->__handlers, object);

    // index == 0/NULL means no handler has been set to this object.
    if (index) {
        lua_getfield(L, LUA_REGISTRYINDEX, LUA_FUNCTION_HANDLER_KEY);
        lua_pushinteger(L, index);
        lua_rawget(L, -2);

        if (stack_set_func) {
            n = stack_set_func(L, ud);
        }
        seal_call(L, n, 0);

        if (cleanup) {
            lua_pushinteger(L, index);
            lua_pushnil(L);
            lua_rawset(L, -3);
        }
    } else {
        // it's ok that there's no handler for certain object. :)
        // fprintf(stderr, "no handler for object (%p)\n", object);
    }
}