#include "lauxlib.h"

#include "SDL.h"
#include "SDL_image.h"

int lsdl_image_load(lua_State* L) {
    const char* file_name = lua_tostring(L, 1);
    if (!file_name) {
        return luaL_argerror(L, 1, "sdl_image.load require a string.");
    }
    
    SDL_Surface* image = IMG_Load(file_name);
    if(!image) {
        return luaL_error(L, "sdl_image.load %s failed.", file_name);
    }
    
    lua_pushlightuserdata(L, image);
    return 1;
}

int luaopen_sdl_image_core(lua_State* L) {
    
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { "load", lsdl_image_load},
        { NULL, NULL },
    };
    
    luaL_newlib(L, lib);

    return 1;
}