#include "lua.h"
#include "lauxlib.h"

#include "seal.h"
#include "texture.h"

EXTERN_GAME;

int ltexture_cache_load(lua_State* L) {
//TODO:     we have process the only the `image` currently
//          *****+image [ui.png]
    
//          +format [RGBA8888]
//
//          +size+h [256.0]
//          |    +w [256.0]
//          +version [1.0]
//          +scale [1]
    
    struct texture_cache* cache = GAME->texture_cache;
    const char* file_name = luaL_checkstring(L, -1);
    if (file_name) {
        struct texture* tex = texture_cache_load(cache, file_name);
        lua_pushinteger(L, tex->id);
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
        { "load_from_cache", ltexture_cache_load },
        { "unload_from_cache", ltexture_cache_unload},
        { NULL, NULL },
    };
    
    luaL_newlib(L, lib);
    return 1;
}