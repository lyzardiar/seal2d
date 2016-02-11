#include "lauxlib.h"

#include "SDL.h"
#include "SDL_image.h"

// usage: sdl_texture.load(file_name, render)
int lsdl_texture_load(lua_State* L) {
    const char* file_name = lua_tostring(L, -2);
    if (!file_name) {
        return luaL_argerror(L, 1, "sdl_texture.load require a string.");
    }
    
    SDL_Renderer* render = lua_touserdata(L, -1);
    if(!render) {
        return luaL_argerror(L, 2, "sdl_texture.load require a userdata(SDL_Renderer).");
    }
    
    SDL_Texture *texture = NULL;
    SDL_Surface *surface = IMG_Load(file_name);
    if (surface) {
        int width = surface->w;
        int height = surface->h;
        texture = SDL_CreateTextureFromSurface(render, surface);
        SDL_FreeSurface(surface);
        if(!texture) {
            return luaL_error(L, "sdl_texture.load texture %s failed.", file_name);
        }
        lua_pushlightuserdata(L, texture);
        lua_pushinteger(L, width);
        lua_pushinteger(L, height);
        return 3;
    } else {
        return luaL_error(L, "sdl_texture.load surface %s failed.", file_name);
    }
}

int lsdl_texture_free(lua_State* L) {
    SDL_Texture* texture = lua_touserdata(L, -1);
    if(!texture) {
        return luaL_argerror(L, 1, "sdl_texture.free require a userdata(SDL_Texture).");
    }
    
    SDL_DestroyTexture(texture);
    return 0;
}

int luaopen_sdl_texture_core(lua_State* L) {
    
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { "load", lsdl_texture_load},
        { "free", lsdl_texture_free},
        { NULL, NULL },
    };
    
    luaL_newlib(L, lib);
    
    return 1;
}