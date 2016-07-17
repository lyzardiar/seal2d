#ifndef COMPAT_H
#define COMPAT_H

#include "lua.h"
#include "lauxlib.h"

#if LUA_VERSION_NUM==501
void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup);
#endif


#ifdef _MSC_VER
extern int strcasecmp(const char *s1, const char *s2);
extern int strncasecmp(const char *s1, const char *s2, register int n);
#endif

#endif