#include "platform/platform.h"

#include "lua.h"
#include "lauxlib.h"

#include "lopen.h"
#include "seal.h"
#include "lua_handler.h"

#ifdef PLAT_DESKTOP

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION

#include "nuklear.h"
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear_glfw_gl3.h"

EXTERN_GAME;

static struct nk_context* ctx = NULL;

#define MAX_VERTEX_BUFFER (512 * 1024)
#define MAX_ELEMENT_BUFFER (128 * 1024)

void nuk_init(void* winctx) {
#ifdef PLAT_DESKTOP
    if (!ctx) {
        ctx = nk_glfw3_init(winctx, NK_GLFW3_DEFAULT);
        struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(&atlas);
        nk_font_atlas_add_from_file(atlas,
                                    GAME->config.nk_gui_font_path,
                                    GAME->config.nk_gui_font_size,
                                    0);
        nk_glfw3_font_stash_end();
    }
#endif
}

int lnuk_new_panel(lua_State* L) {
    struct nk_panel* panel = STRUCT_NEW(nk_panel);
    lua_pushlightuserdata(L, panel);
    return 1;
}

int lnuk_free_panel(lua_State* L) {
    return 0;
}

int lnuk_draw_start(lua_State* L) {
    nk_glfw3_new_frame();
    return 0;
}

int lnuk_draw_end(lua_State* L) {
    nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
    return 0;
}

int lnk_begin(lua_State* L) {
    const char* panel_title = luaL_checkstring(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    lua_pushvalue(L, 2);
    
    struct nk_rect rect = {
        getfield_i(L, "x"),
        getfield_i(L, "y"),
        getfield_i(L, "w"),
        getfield_i(L, "h"),
    };
    
    lua_pop(L, 1);
    uint32_t flags = luaL_checkinteger(L, 3);

    static struct nk_panel panel;
    bool ret = nk_begin(ctx, &panel, panel_title, rect, flags);
    lua_pushboolean(L, ret);

    return 1;
}

int lnk_end(lua_State* L) {
    nk_end(ctx);
    return 0;
}

int lnk_layout_row_static(lua_State* L) {
    nk_layout_row_static(ctx,
                         luaL_checknumber(L, 1),
                         luaL_checknumber(L, 2),
                         luaL_checknumber(L, 3));
    return 0;
}

int lnk_layout_row_dynamic(lua_State* L) {
    nk_layout_row_dynamic(ctx,
                          luaL_checknumber(L, 1),
                          luaL_checknumber(L, 2));

    return 0;
}

int lnk_button_label(lua_State* L) {
    lua_pushboolean(L, nk_button_label(ctx,
                                       luaL_checkstring(L, 1),
                                       luaL_checkinteger(L, 2)));
    return 1;
}

int lnk_option_label(lua_State* L) {
    
    return 1;
}

int lnk_property_int(lua_State* L) {
    
    return 0;
}


// we support only 1 nk_node.
int luaopen_nuklear_core(lua_State* L) {
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { "nuk_new_panel", lnuk_new_panel },
        { "nuk_free_panel", lnuk_free_panel },
        { "nuk_draw_start", lnuk_draw_start },
        { "nuk_draw_end", lnuk_draw_end },

        { "nk_begin", lnk_begin },
        { "nk_end", lnk_end },
        { "nk_layout_row_static", lnk_layout_row_static },
        { "nk_button_label", lnk_button_label },
        { "nk_layout_row_dynamic", lnk_layout_row_dynamic },
        { "nk_option_label", lnk_option_label },
        { "nk_property_int", lnk_property_int },
        { NULL, NULL },
    };
    
    luaL_newlib(L, lib);
    
    return 1;
}
#else
// make this compliable on mobile platform
    int luaopen_nuklear_core(lua_State* L) {
        printf("nuklear not implemented on mobile platform.\n");
        return 0;
    }
#endif