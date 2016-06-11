#include "lua.h"
#include "lauxlib.h"


int luaopen_nuklear(lua_State* L) {
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { NULL, NULL },
    };
    
    luaL_newlib(L, lib);
    
    return 1;
}