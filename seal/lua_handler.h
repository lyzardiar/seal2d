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


#ifndef lua_handler_h
#define lua_handler_h

#include "seal_base.h"

// WC - todo fix

// #include "lua.h"

struct Hashmap;

#define LUA_FUNCTION_HANDLER_KEY "LUA_FUNCTION_HANDLER_KEY"

struct lua_handler {
    struct Hashmap* __handlers;
    unsigned int __func_index;
};

typedef unsigned long LUA_FUNCTION_HANDLER;

struct lua_handler* lua_handler_new(lua_State* L);
void lua_handler_free(struct lua_handler* self);

void lua_handler_set(struct lua_handler* self,
                     void* object,
                     LUA_FUNCTION_HANDLER lua_function);

void lua_handler_clean(struct lua_handler* self,
                       lua_State* L,
                       void* object);

unsigned int lua_handler_new_func(struct lua_handler* self,
                                  lua_State* L,
                                  void* object,
                                  int func_loc);

void lua_handler_exe_func(struct lua_handler* self,
                          lua_State* L,
                          void* object,
                          int (*stack_set_func)(lua_State*, void* ud),
                          void* ud,
                          bool cleanup);

#endif
