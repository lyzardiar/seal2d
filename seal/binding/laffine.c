#include "math/affine.h"
#include "lopen.h"

static struct affine* self(lua_State* L, int index) {
    struct affine* af = lua_touserdata(L, index);
    if(!af) {
        luaL_argerror(L, 1, "affine.free require a userdata(struct affine*).");
        return NULL;
    }
    return af;
}
int laf_alloc(lua_State* L) {
    struct affine* af = af_alloc();
    lua_pushlightuserdata(L, af);
    return 1;
}

int laf_free(lua_State* L) {
    af_free(self(L, -1));
    return 0;
}

int laf_translate(lua_State* L) {
    struct affine* af = self(L, -3);
    float x = lua_tonumber(L, -2);
    float y = lua_tonumber(L, -1);
    
    af_translate(af, x, y);
    return 0;
}

int laf_rotate(lua_State* L) {
    struct affine* af = self(L, -2);
    float r = lua_tonumber(L, -1);
    
    af_rotate(af, r);
    return 0;
}

int laf_scale(lua_State* L) {
    struct affine* af = self(L, -3);
    float sx = lua_tonumber(L, -2);
    float sy = lua_tonumber(L, -1);
    
    af_scale(af, sx, sy);
    
    return 0;
}

int laf_concat(lua_State* L) {
    struct affine* m1 = self(L, -2);
    struct affine* m2 = self(L, -1);
    
    af_concat(m1, m2);
    return 0;
}

int laf_serial_t(lua_State* L) {
    struct affine* af = self(L, -1);
    
    lua_newtable(L);
    setfiled_f(L, "x", af->x);
    setfiled_f(L, "y", af->y);
    setfiled_f(L, "a", af->a);
    setfiled_f(L, "b", af->b);
    setfiled_f(L, "c", af->c);
    setfiled_f(L, "d", af->d);

    return 1;
}

int luaopen_affine_core(lua_State* L) {
    
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { "alloc",      laf_alloc},
        { "free",       laf_free},
        { "translate",  laf_translate},
        { "rotate",     laf_rotate},
        { "scale",      laf_scale},
        { "concat",     laf_concat},
        { "serial_t",   laf_serial_t},
        { NULL, NULL },
    };
    
    luaL_newlib(L, lib);
    
    return 1;
}