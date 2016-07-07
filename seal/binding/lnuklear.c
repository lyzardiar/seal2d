#include "lua.h"
#include "lauxlib.h"

#include "nuk_node.h"

#define NK_INCLUDE_FIXED_TYPES
#include "nuklear.h"
#include "lopen.h"
#include "seal.h"
#include "lua_handler.h"

EXTERN_GAME;

int lnuk_node_register(lua_State* L) {
    lua_handler_set_func(GAME->lua_handler, L, GAME->nuk_node, 1);
    return 0;
}

int lnk_begin(lua_State* L) {
    struct nk_context* ctx = global_nk_context();
    struct nk_panel* panel = GAME->nuk_node->panel;
    
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

    bool ret = nk_begin(ctx, panel, panel_title, rect, flags);
    lua_pushboolean(L, ret);

    return 1;
}

int lnk_end(lua_State* L) {
    nk_end(global_nk_context());
    return 0;
}

int lnk_layout_row_static(lua_State* L) {
    nk_layout_row_static(global_nk_context(),
                         luaL_checknumber(L, 1),
                         luaL_checknumber(L, 2),
                         luaL_checknumber(L, 3));
    return 0;
}

int lnk_layout_row_dynamic(lua_State* L) {
    nk_layout_row_dynamic(global_nk_context(),
                          luaL_checknumber(L, 1),
                          luaL_checknumber(L, 2));

    return 0;
}

int lnk_button_label(lua_State* L) {
    lua_pushboolean(L, nk_button_label(global_nk_context(),
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
        { "nuk_node_register", lnuk_node_register},
        { "nk_begin", lnk_begin},
        { "nk_end", lnk_end},
        { "nk_layout_row_static", lnk_layout_row_static},
        { "nk_button_label", lnk_button_label},
        { "nk_layout_row_dynamic", lnk_layout_row_dynamic},
        { "nk_option_label", lnk_option_label},
        { "nk_property_int", lnk_property_int},
        { NULL, NULL },
    };
    
    luaL_newlib(L, lib);
    
    return 1;
}