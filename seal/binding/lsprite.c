#include <string.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "lopen.h"
#include "sprite.h"
#include "memory.h"
#include "util.h"

//void PrintTable(lua_State *L)
//{
//    lua_pushnil(L);
//    
//    while(lua_next(L, -2) != 0)
//    {
//        if(lua_isstring(L, -1))
//            printf("%s = %s\n", lua_tostring(L, -2), lua_tostring(L, -1));
//        else if(lua_isnumber(L, -1))
//            printf("%s = %.2f\n", lua_tostring(L, -2), lua_tonumber(L, -1));
//        else if(lua_istable(L, -1))
//            PrintTable(L);
//        
//        lua_pop(L, 1);
//    }
//}
//

int lsprite_load_spriteframe(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    
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

int lsprite_free(lua_State* L) {
    s_free(lua_touserdata(L, 1));
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
        { "free", lsprite_free },
        { NULL, NULL },
    };
    
    luaL_newlib(L, lib);
    
    return 1;
}