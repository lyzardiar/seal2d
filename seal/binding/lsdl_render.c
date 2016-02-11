#include "lauxlib.h"

#include "seal.h"

extern struct game* GAME;

void stackDump (lua_State *L);

int lsdl_render_clear(lua_State* L) {
    SDL_RenderClear(GAME->gl_render);
    return 0;
}

int lsdl_render_draw(lua_State* L) {
    SDL_Texture* texture = lua_touserdata(L, 1);
    if(!texture) {
        return luaL_argerror(L, 1, "lsdl_render_draw require a texture, but it's null.");
    }
    
    float sx = lua_tonumber(L, 2);
    float sy = lua_tonumber(L, 3);
    float sw = lua_tonumber(L, 4);
    float sh = lua_tonumber(L, 5);
    
    float x = lua_tonumber(L, 6);
    float y = lua_tonumber(L, 7);
    float w = lua_tonumber(L, 8);
    float h = lua_tonumber(L, 9);
    
    SDL_Rect src_rect = {
        sx, sy, sw, sh,
    };
    SDL_Rect dst_rect = {
        x, y, w, h
    };
    
    SDL_RenderCopy(GAME->gl_render, texture, &src_rect, &dst_rect);
    return 0;
}

int lsdl_render_draw_ex(lua_State* L){
    SDL_Texture* texture = lua_touserdata(L, 1);
    if(!texture) {
        return luaL_argerror(L, 1, "lsdl_render_draw require a texture, but it's null.");
    }
    
    float sx = lua_tonumber(L, 2);
    float sy = lua_tonumber(L, 3);
    float sw = lua_tonumber(L, 4);
    float sh = lua_tonumber(L, 5);
    
    float x = lua_tonumber(L, 6);
    float y = lua_tonumber(L, 7);
    float w = lua_tonumber(L, 8);
    float h = lua_tonumber(L, 9);
    
    float angle = lua_tonumber(L, 10);
    int center_x = (int)lua_tonumber(L, 11);
    int center_y = (int)lua_tonumber(L, 12);
    int flip = lua_tonumber(L, 13);
    
    SDL_Rect src_rect = {
        sx, sy, sw, sh,
    };
    SDL_Rect dst_rect = {
        x, y, w, h
    };
    SDL_Point center_point = {
        center_x, center_y
    };
    
    SDL_RenderCopyEx(GAME->gl_render, texture, &src_rect, &dst_rect, angle, &center_point, flip);

    return 0;
}

int lsdl_render_present(lua_State* L) {
    SDL_RenderPresent(GAME->gl_render);
    return 0;
}

int luaopen_sdl_render_core(lua_State* L) {
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { "clear", lsdl_render_clear},
        { "draw", lsdl_render_draw},
        { "draw_ex", lsdl_render_draw_ex},
        { "present", lsdl_render_present},
        { NULL, NULL },
    };
    
    luaL_newlib(L, lib);
    
    return 1;
}