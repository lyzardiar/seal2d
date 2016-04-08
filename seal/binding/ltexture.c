#include "lua.h"
#include "lauxlib.h"

#include "texture.h"

int ltexture_load(lua_State* L) {
    const char* file_name = luaL_checkstring(L, -1);
    if (file_name) {
        struct texture* tex = texture_load_from_png(file_name);
        lua_pushlightuserdata(L, tex);
        return 1;
    }
    
    return 0;
}

int luaopen_seal_texture(lua_State* L) {
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { "load", ltexture_load },
        { NULL, NULL },
    };
    
    luaL_newlib(L, lib);
    return 1;
}