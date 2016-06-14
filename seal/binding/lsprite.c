#include <string.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "lopen.h"
#include "sprite.h"
#include "memory.h"
#include "util.h"

EXTERN_GAME;

int lsprite_load_spriteframe(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TTABLE);
    
    // read the info in the frame
    struct sprite_frame* frame = sprite_frame_new();
    memset(frame, 0, sizeof(struct sprite_frame));
    
    frame->rotated = (int)getfield_i(L, "rotated");
    frame->trimmed = (int)getfield_i(L, "trimmed");

    lua_pushstring(L, "frame");
    lua_gettable(L, 1);
    
    struct rect* frame_rect = &frame->frame_rect;
    frame_rect->x = (int)getfield_i(L, "x");
    frame_rect->y = (int)getfield_i(L, "y");
    frame_rect->width = (int)getfield_i(L, "w");
    frame_rect->height = (int)getfield_i(L, "h");
    lua_pop(L, 1);
    
    lua_pushstring(L, "spriteSourceSize");
    lua_gettable(L, 1);
    
    struct rect* source_rect = &frame->source_rect;
    source_rect->x = (int)getfield_i(L, "x");
    source_rect->y = (int)getfield_i(L, "y");
    source_rect->width = (int)getfield_i(L, "w");
    source_rect->height = (int)getfield_i(L, "h");
    lua_pop(L, 1);
    
    lua_pushstring(L, "sourceSize");
    lua_gettable(L, 1);

    struct size* size = &frame->source_size;
    size->width = (int)getfield_i(L, "w");
    size->height = (int)getfield_i(L, "h");
    lua_pop(L, 1);
    
    // read the info in the meta
    lua_pushstring(L, "size");
    lua_gettable(L, 2);

    float texture_width = getfield_f(L, "w");
    float texture_height = getfield_f(L, "h");

    sprite_frame_init_uv(frame, texture_width, texture_height);
    
    lua_pop(L, -1);
    lua_pushlightuserdata(L, frame);
    return 1;
}

int lsprite_unload_spriteframe(lua_State* L) {
    struct sprite_frame* self = lua_touserdata(L, 1);
    s_free(self);
    return 0;
}

int lsprite_set_texture_id(lua_State* L) {
    struct sprite_frame* self = lua_touserdata(L, 1);
    self->tex_id = (GLuint)lua_tointeger(L, 2);
    s_assert(self->tex_id > 0);
    return 0;
}


int lsprite_new(lua_State* L) {
    struct sprite_frame* frame = lua_touserdata(L, 1);
    struct sprite* sprite = sprite_new(frame);
    lua_pushlightuserdata(L, sprite);
    return 1;
}

int lsprite_new_container(lua_State* L) {
    lua_Number x, y, width, height;
    if(lua_gettop(L) == 4) {
        x = luaL_checknumber(L, 1);
        y = luaL_checknumber(L, 2);
        width = luaL_checknumber(L, 3);
        height = luaL_checknumber(L, 4);
    } else {
        x = y = 0;
        width = GAME->config.window_width;
        height = GAME->config.window_height;
    }
    
    struct rect r = {
        x, y, width, height
    };
    
    struct sprite* s = sprite_new_container(&r);
    lua_pushlightuserdata(L, s);
    return 1;
}

int lsprite_free(lua_State* L) {
    s_free(lua_touserdata(L, 1));
    return 0;
}

int lsprite_set_pos(lua_State* L) {
    struct sprite* self = lua_touserdata(L, 1);
    lua_Number x = luaL_checknumber(L, 2);
    lua_Number y = luaL_checknumber(L, 3);
    sprite_set_pos(self, x, y);
    return 0;
}

int lsprite_set_rotation(lua_State* L) {
    struct sprite* self = lua_touserdata(L, 1);
    lua_Number rotation = luaL_checknumber(L, 2);
    sprite_set_rotation(self, rotation);
    return 0;
}

int lsprite_set_scale(lua_State* L) {
    struct sprite* self = lua_touserdata(L, 1);
    lua_Number scale = luaL_checknumber(L, 2);
    sprite_set_scale(self, scale);
    return 0;
}

int lsprite_add_child(lua_State* L) {

    luaL_argcheck(L, lua_isuserdata(L, 1), 1, "sprite expected for arg 1");
    luaL_argcheck(L, lua_isuserdata(L, 2), 2, "sprite expected for arg 2");

    struct sprite* self = lua_touserdata(L, 1);
    struct sprite* child = lua_touserdata(L, 2);
    sprite_add_child(self, child);
    
    return 0;
}

int luaopen_seal_sprite(lua_State* L) {
    
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { "spriteframe_load", lsprite_load_spriteframe },
        { "spriteframe_unload", lsprite_unload_spriteframe },
        { "spriteframe_set_texture_id", lsprite_set_texture_id},
        
        { "new", lsprite_new },
        { "new_container", lsprite_new_container },
        { "free", lsprite_free },
        { "set_pos", lsprite_set_pos },
        { "set_rotation", lsprite_set_rotation },
        { "set_scale", lsprite_set_scale },
        { "add_child", lsprite_add_child },
        { NULL, NULL },
    };
    
    luaL_newlib(L, lib);
    
    return 1;
}