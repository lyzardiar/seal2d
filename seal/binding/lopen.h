#ifndef __seal__lopen__
#define __seal__lopen__

#include "lauxlib.h"

void stackDump (lua_State* L);

void setfiled_f(lua_State* L, const char* key, float n);
void setfiled_i(lua_State* L, const char* key, lua_Integer n);
void setfiled_s(lua_State* L, const char* key, const char* s);

lua_Integer getfield_i(lua_State* L, const char* key);
lua_Number  getfield_f(lua_State* L, const char* key);
const char* getfiled_s(lua_State* L, const char* key);

void getarray_f(lua_State* L, const char* key, float* f_array, int cnt);
void getarray_i(lua_State* L, const char* key, int* i_array, int cnt);

#define bind_func_implemention_v(bind, to, ctx) \
    int l##bind(lua_State* L) { \
        to(ctx);\
        return 0; \
    }

#define bind_func_implemention_i1(bind, to, ctx)  \
    int l##bind(lua_State* L) { \
    to(ctx, luaL_checkinteger(L, 1));\
    return 0; \
}

#define bind_func_implemention_f1(bind, to, ctx)  \
    int l##bind(lua_State* L) { \
    to(ctx, luaL_checknumber(L, 1));\
    return 0; \
}

#define bind_func_implemention_f2(bind, to, ctx)  \
    int l##bind(lua_State* L) { \
    to(ctx, luaL_checknumber(L, 1), luaL_checknumber(L, 2));\
    return 0; \
}

#endif
