#include "lua.h"
#include "lauxlib.h"

#include "texture.h"

int ltexture_cache_load(lua_State* L) {
    const char* file_name = luaL_checkstring(L, -1);
    if (file_name) {
        struct texture* tex = texture_load_from_png(file_name);
        lua_pushlightuserdata(L, tex);
        return 1;
    }
    
    return 0;
}

int ltexture_cache_unload(lua_State* L) {
    struct texture_cache* self = lua_touserdata(L, 1);
    const char* file_name = luaL_checkstring(L, 2);
    texture_cache_unload(self, file_name);
    return 1;
}

//------------------------------------------------------------

int luaopen_seal_texture(lua_State* L) {
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { "cache_load", ltexture_cache_load },
        { "cache_unload", ltexture_cache_unload},
        { NULL, NULL },
    };
    
    luaL_newlib(L, lib);
    return 1;
}