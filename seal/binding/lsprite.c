#include <string.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "lopen.h"
#include "anim.h"
#include "sprite.h"
#include "memory.h"
#include "util.h"
#include "lua_handler.h"

EXTERN_GAME;

int lsprite_frame_cache_get(lua_State* L)
{
    const char* key = luaL_checkstring(L, 1);
    struct sprite_frame* frame =
                    sprite_frame_cache_get(GAME->sprite_frame_cache, key);
    lua_pushlightuserdata(L, frame);
    return 1;
}

int lsprite_load_spriteframe(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TTABLE);

    const char* key = luaL_checkstring(L, 3);
    struct sprite_frame* frame =
                    sprite_frame_cache_get(GAME->sprite_frame_cache, key);
    if (!frame->__initialized) {
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
    }

    lua_pop(L, -1);
    lua_pushlightuserdata(L, frame);
    return 1;
}

int lsprite_unload_spriteframe(lua_State* L)
{
    struct sprite_frame* self = lua_touserdata(L, 1);
    s_free(self);
    return 0;
}

int lsprite_set_texture_id(lua_State* L)
{
    struct sprite_frame* self = lua_touserdata(L, 1);
    self->tex_id = (GLuint)lua_tointeger(L, 2);
    s_assert(self->tex_id > 0);
    return 0;
}


int lsprite_new(lua_State* L)
{
    struct sprite_frame* frame = lua_touserdata(L, 1);
    struct sprite* sprite = sprite_new(frame);
    lua_pushlightuserdata(L, sprite);
    return 1;
}

int lsprite_new_label(lua_State* L)
{
    const char* label = luaL_checkstring(L, 1);
    struct sprite* sprite = sprite_new_label(label);
    lua_pushlightuserdata(L, sprite);
    return 1;
}

int lsprite_new_bmfont_label(lua_State* L)
{
    const char* label = luaL_checkstring(L, 1);
    const char* fnt_path = luaL_checkstring(L, 2);
    struct sprite* sprite = sprite_new_bmfont_label(label, fnt_path);
    lua_pushlightuserdata(L, sprite);
    return 1;
}

static void check_rect(lua_State* L, struct rect* r)
{
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

    r->x = x;
    r->y = y;
    r->width = width;
    r->height = height;
}

int lsprite_new_container(lua_State* L)
{
    struct rect r;
    check_rect(L, &r);

    struct sprite* s = sprite_new_container(&r);
    lua_pushlightuserdata(L, s);
    return 1;
}

int lsprite_new_spine(lua_State* L)
{
    const char* atlas_path = luaL_checkstring(L, 1);
    const char* spine_data_path = luaL_checkstring(L, 2);
    lua_Number scale = luaL_checknumber(L, 3);

    struct sprite* s = sprite_new_spine(atlas_path, spine_data_path, scale);
    lua_pushlightuserdata(L, s);

    return 1;
}

static int new_line(lua_State* L)
{
    float vertex[4] = {0};
    getarray_f(L, "vertex", vertex, 4);

    int color[4] = {0};
    getarray_i(L, "color", color, 4);

    lua_Number width = getfield_f(L, "width");

    struct sprite* line = sprite_new_line(vertex, width, C4B_COLOR_A(color));
    lua_pushlightuserdata(L, line);
    return 1;
}

static int new_rect(lua_State* L)
{
    int r[4] = {0};
    getarray_i(L, "rect", r, 4);

    int fill_color[4] = {0};
    getarray_i(L, "fill_color", fill_color, 4);

    int outline_color[4] = {0};
    getarray_i(L, "outline_color", outline_color, 4);

    struct rect rect = {
        r[0], r[1], r[2], r[3]
    };

    struct sprite* s = sprite_new_rect(&rect,
                                       FILL_SOLID,
                                       C4B_COLOR_A(fill_color),
                                       C4B_COLOR_A(outline_color));
    lua_pushlightuserdata(L, s);
    return 1;
}

int lsprite_new_primitive(lua_State* L) {
    char type = lua_tostring(L, 1)[0];

    int ret = 0;
    switch (type) {
        case 'L':
            ret = new_line(L);
            break;
        case 'R':
            ret = new_rect(L);
            break;
        default:
            break;
    }

    return ret;
}

int lsprite_new_clip(lua_State* L)
{
    struct rect r;
    check_rect(L, &r);

    struct sprite* s = sprite_new_clip(&r);
    lua_pushlightuserdata(L, s);
    return 1;
}

int lsprite_free(lua_State* L)
{
    sprite_free(lua_touserdata(L, 1));
    return 0;
}

int lsprite_set_text(lua_State* L)
{
    struct sprite* self = (struct sprite*)lua_touserdata(L, 1);
    const char* label = luaL_checkstring(L, 2);
    sprite_set_text(self, label);
    return 0;
}

int lsprite_set_size(lua_State* L)
{
    struct sprite* self = (struct sprite*)lua_touserdata(L, 1);
    lua_Integer width = luaL_checkinteger(L, 2);
    lua_Integer height = luaL_checkinteger(L, 3);
    sprite_set_size(self, width, height);
    return 0;
}

int lsprite_register_handler(lua_State* L)
{
    struct sprite* self = (struct sprite*)lua_touserdata(L, 1);
    lua_handler_new_func(GAME->lua_handler, L, self, 2);

    return 0;
}

int lsprite_clean_handler(lua_State* L)
{
    struct sprite* self = (struct sprite*)lua_touserdata(L, 1);
    lua_handler_clean(GAME->lua_handler, L, self);
    return 0;
}

int lsprite_run_action(lua_State* L)
{
    struct sprite* self = (struct sprite*)lua_touserdata(L, 1);
    struct action* action = (struct action*)lua_touserdata(L, 2);
    sprite_run_action(self, action);
    return 0;
}

int lsprite_set_anim(lua_State* L)
{
    struct sprite* self = (struct sprite*)lua_touserdata(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);

    int len = luaL_len(L, 2);
    struct array* frames = array_new(len);
    for (int i = 1; i <= len; ++i) {
        lua_rawgeti(L, -1, i);
        struct sprite_frame* frame = lua_touserdata(L, -1);
        array_push_back(frames, frame);
        lua_pop(L, 1);
    }

    struct anim* anim = anim_new(frames);
    sprite_set_anim(self, anim);

    array_free(frames);
    return 0;
}

int lsprite_set_spine_anim(lua_State* L)
{
    struct sprite* self = (struct sprite*)lua_touserdata(L, 1);
    const char* anim_name = luaL_checkstring(L, 2);
    int track = luaL_checknumber(L, 3);
    bool loop = lua_toboolean(L, 4);

    sprite_set_spine_anim(self, anim_name, track, loop);
    return 0;
}

int lsprite_set_anim_interval(lua_State* L)
{
    struct sprite* self = lua_touserdata(L, 1);
    float interval = luaL_checknumber(L, 2);
    anim_set_interval(self->anim, interval);
    return 0;
}

int lsprite_set_visible(lua_State* L)
{
    struct sprite* self = lua_touserdata(L, 1);
    sprite_set_visible(self, lua_toboolean(L, 2));
    return 0;
}

int lsprite_set_pos(lua_State* L)
{
    struct sprite* self = lua_touserdata(L, 1);
    lua_Number x = luaL_checknumber(L, 2);
    lua_Number y = luaL_checknumber(L, 3);
    sprite_set_pos(self, x, y);
    return 0;
}

int lsprite_set_anchor(lua_State* L)
{
    struct sprite* self = lua_touserdata(L, 1);
    lua_Number x = luaL_checknumber(L, 2);
    lua_Number y = luaL_checknumber(L, 3);
    sprite_set_anchor(self, x, y);
    return 0;
}

int lsprite_set_rotation(lua_State* L)
{
    struct sprite* self = lua_touserdata(L, 1);
    lua_Number rotation = luaL_checknumber(L, 2);
    sprite_set_rotation(self, rotation);
    return 0;
}

int lsprite_set_scale(lua_State* L)
{
    struct sprite* self = lua_touserdata(L, 1);
    lua_Number scale = luaL_checknumber(L, 2);
    sprite_set_scale(self, scale);
    return 0;
}

int lsprite_set_color(lua_State* L)
{
    struct sprite* self = lua_touserdata(L, 1);
    lua_Integer r = lua_tonumber(L, 2);
    lua_Integer g = lua_tonumber(L, 3);
    lua_Integer b = lua_tonumber(L, 4);
    lua_Integer a = lua_tonumber(L, 5);
    sprite_set_color(self, C4B_COLOR(r,g,b,a));
    return 0;
}

int lsprite_get_pos(lua_State* L)
{
    struct sprite* self = lua_touserdata(L, 1);
    lua_pushnumber(L, self->x);
    lua_pushnumber(L, self->y);
    return 2;
}

int lsprite_get_size(lua_State* L)
{
    struct sprite* self = lua_touserdata(L, 1);
    lua_pushnumber(L, self->width);
    lua_pushnumber(L, self->height);
    return 2;
}

int lsprite_get_glyph(lua_State* L)
{
    struct sprite* self = lua_touserdata(L, 1);
    struct glyph* g = sprite_get_glyph(self);

    lua_newtable(L);
    setarray_n(L, 1, g->bl.position[0]);
    setarray_n(L, 2, g->bl.position[1]);
    setarray_n(L, 3, g->br.position[0]);
    setarray_n(L, 4, g->br.position[1]);
    setarray_n(L, 5, g->tr.position[0]);
    setarray_n(L, 6, g->tr.position[1]);
    setarray_n(L, 7, g->tl.position[0]);
    setarray_n(L, 8, g->tl.position[1]);

    return 1;
}

int lsprite_add_child(lua_State* L)
{
    luaL_argcheck(L, lua_isuserdata(L, 1), 1, "sprite expected for arg 1");
    luaL_checktype(L, 2, LUA_TTABLE);

    lua_getfield(L, 2, "__cobj");
    struct sprite* child = lua_touserdata(L, -1);

    struct sprite* self = lua_touserdata(L, 1);
    sprite_add_child(self, child);

    return 0;
}

int lsprite_remove_all_child(lua_State* L)
{
    sprite_remove_all_child(lua_touserdata(L, 1));
    return 0;
}

int luaopen_seal_sprite(lua_State* L)
{
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {

        { "get_frame_from_cache", lsprite_frame_cache_get},

        { "load_sprite_frame", lsprite_load_spriteframe },
        { "unload_sprite_frame", lsprite_unload_spriteframe },
        { "set_frame_texture_id", lsprite_set_texture_id},

        { "new", lsprite_new },
        { "new_label", lsprite_new_label },
        { "new_bmfont_label", lsprite_new_bmfont_label },
        { "new_container", lsprite_new_container },
        { "new_spine", lsprite_new_spine },
        { "new_primitive", lsprite_new_primitive },
        { "new_clip", lsprite_new_clip },
        { "free", lsprite_free },
        { "set_text", lsprite_set_text },
        { "register_handler", lsprite_register_handler },
        { "clean_handler", lsprite_clean_handler },
        { "run_action", lsprite_run_action },
        { "set_anim", lsprite_set_anim },
        { "set_spine_anim", lsprite_set_spine_anim},
        { "set_anim_interval", lsprite_set_anim_interval},
        { "set_visible", lsprite_set_visible },
        { "set_pos", lsprite_set_pos },
        { "set_anchor", lsprite_set_anchor },
        { "set_rotation", lsprite_set_rotation },
        { "set_scale", lsprite_set_scale },
        { "set_color", lsprite_set_color },
        { "set_size", lsprite_set_size },
        { "get_pos", lsprite_get_pos},
        { "get_size", lsprite_get_size},
        { "get_glyph", lsprite_get_glyph},
        { "add_child", lsprite_add_child },
        { "remove_all_child", lsprite_remove_all_child},
        { NULL, NULL },
    };

    luaL_newlib(L, lib);

    return 1;
}