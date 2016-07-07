#ifndef lua_handler_h
#define lua_handler_h

#include "lua.h"

struct Hashmap;

#define LUA_FUNCTION_HANDLER_KEY "LUA_FUNCTION_HANDLER_KEY"

struct lua_handler {
    struct Hashmap* __handlers;
    unsigned int __func_index;
};

typedef unsigned long LUA_FUNCTION_HANDLER;

struct lua_handler* lua_handler_new(lua_State* L);
void lua_handler_free(struct lua_handler* self);

void lua_handler_set(struct lua_handler* self, void* object, LUA_FUNCTION_HANDLER lua_function);

void lua_handler_clean(struct lua_handler* self, void* object);

unsigned int lua_handler_set_func(struct lua_handler* self, lua_State* L, void* object, int func_loc);

void lua_handler_exe_func(struct lua_handler* self,
                          lua_State* L,
                          void* object,
                          int (*stack_set_func)(lua_State*, void* ud),
                          void* ud);

#endif
