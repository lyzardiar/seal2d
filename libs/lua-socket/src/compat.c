#include "compat.h"
#include <ctype.h>

#if LUA_VERSION_NUM==501
/*
** Adapted from Lua 5.2
*/
void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
  luaL_checkstack(L, nup+1, "too many upvalues");
  for (; l->name != NULL; l++) {  /* fill the table with given functions */
    int i;
    lua_pushstring(L, l->name);
    for (i = 0; i < nup; i++)  /* copy upvalues to the top */
      lua_pushvalue(L, -(nup+1));
    lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
    lua_settable(L, -(nup + 3));
  }
  lua_pop(L, nup);  /* remove upvalues */
}
#endif

#ifdef _MSC_VER
int strcasecmp(const char *s1, const char *s2)
{
	while (toupper((unsigned char)*s1) == toupper((unsigned char)*s2++))
		if (*s1++ == '\0') return 0;
	return(toupper((unsigned char)*s1) - toupper((unsigned char)*--s2));
}

int strncasecmp(const char *s1, const char *s2, register int n)
{
	while (--n >= 0 && toupper((unsigned char)*s1) == toupper((unsigned char)*s2++))
		if (*s1++ == '\0')  return 0;
	return(n < 0 ? 0 : toupper((unsigned char)*s1) - toupper((unsigned char)*--s2));
}
#endif