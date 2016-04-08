#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "lopen.h"

#include "sprite.h"

void PrintTable(lua_State *L)
{
    lua_pushnil(L);
    
    while(lua_next(L, -2) != 0)
    {
        if(lua_isstring(L, -1))
            printf("%s = %s\n", lua_tostring(L, -2), lua_tostring(L, -1));
        else if(lua_isnumber(L, -1))
            printf("%s = %d\n", lua_tostring(L, -2), lua_tonumber(L, -1));
        else if(lua_istable(L, -1))
            PrintTable(L);
        
        lua_pop(L, 1);
    }
}


int lsprite_load_spriteframe(lua_State* L) {

    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TTABLE);

    struct sprite_frame* frame =
        // parse the spriteframe data

        lua_pushstring(L, "frame");
        lua_gettable(L, 1);
    
        int x = getfield_i(L, "x");
        int y = getfield_i(L, "y");
        int w = getfield_i(L, "w");
        int h = getfield_i(L, "h");

        
        printf("x, y, w, h = %d, %d, %d, %d\n", x, y, w, h);
        return  1;
}

int lsprite_unload_spriteframe(lua_State* L) {
    
    return 1;
}


int luaopen_seal_sprite(lua_State* L) {
    
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { "load_spriteframe", lsprite_load_spriteframe },
        { "unload_spriteframe", lsprite_unload_spriteframe },
        { NULL, NULL },
    };
    
    luaL_newlib(L, lib);
    
    
    return 1;
}