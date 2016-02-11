#ifndef __seal__lopen__
#define __seal__lopen__

#include "lauxlib.h"

void stackDump (lua_State *L);
void setfiled_f(lua_State *L, const char* key, float n);
void setfiled_i(lua_State* L, const char* key, lua_Integer n);
void setfiled_s(lua_State *L, const char* key, const char* s);

#endif
