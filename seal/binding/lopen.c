#include "lauxlib.h"
#include "platform/platform.h"
// core engine part
extern int luaopen_seal_core(lua_State* L);
extern int luaopen_seal_platform(lua_State* L);
extern int luaopen_seal_texture(lua_State* L);
extern int luaopen_seal_sprite(lua_State* L);

// third part part.
extern int luaopen_socket_core(lua_State *L);
extern int luaopen_mime_core(lua_State *L);
extern int luaopen_cjson(lua_State* L);
extern int luaopen_zlib(lua_State* L);

extern int luaopen_nuklear_core(lua_State* L);
extern int luaopen_nanovg_core(lua_State* L);

void stackDump (lua_State *L) {
    int i;
    int top = lua_gettop(L);
    for (i = 1; i <= top; i++) {  /* repeat for each level */
        int t = lua_type(L, i);
        switch (t) {
                
            case LUA_TSTRING:  /* strings */
                printf("`%s'", lua_tostring(L, i));
                break;
                
            case LUA_TBOOLEAN:  /* booleans */
                printf(lua_toboolean(L, i) ? "true" : "false");
                break;
                
            case LUA_TNUMBER:  /* numbers */
                printf("%g", lua_tonumber(L, i));
                break;
                
            default:  /* other values */
                printf("%s", lua_typename(L, t));
                break;
                
        }
        printf("    ");  /* put a separator */
    }
    printf("\n");  /* end the listing */
}

void setfiled_f(lua_State *L, const char* key, float f) {
    lua_pushstring(L, key);
    lua_pushnumber(L, f);
    lua_settable(L, -3);
}

void setfiled_i(lua_State* L, const char* key, lua_Integer n) {
    lua_pushstring(L, key);
    lua_pushinteger(L, n);
    lua_settable(L, -3);
}

void setfiled_s(lua_State *L, const char* key, const char* s) {
    lua_pushstring(L, key);
    lua_pushstring(L, s);
    lua_settable(L, -3);
}

lua_Integer getfield_i(lua_State* L, const char* key) {
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    lua_Integer value = lua_tointeger(L, -1);
    lua_pop(L, 1);
    return value;
}

lua_Number getfield_f(lua_State* L, const char* key) {
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    lua_Integer value = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return value;
}

const char* getfiled_s(lua_State* L, const char* key) {
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    const char* value = lua_tostring(L, -1);
    lua_pop(L, 1);
    return value;
}

void getarray_f(lua_State* L, const char* key, float* f_array, int cnt) {
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    if(!lua_isnil(L, -1)) {
        for (int i = 0; i < cnt; ++i) {
            lua_rawgeti(L, -1, i+1);
            f_array[i] = lua_tonumber(L, -1);
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
}

void getarray_i(lua_State* L, const char* key, int* f_array, int cnt) {
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    if (!lua_isnil(L, -1)) {
        for (int i = 0; i < cnt; ++i) {
            lua_rawgeti(L, -1, i+1);
            f_array[i] = lua_tointeger(L, -1);
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
}

void luaopen_lua_extensions(lua_State *L)
{
    // load extensions
    luaL_Reg lua_modules[] = {
        { "engine_core", luaopen_seal_core },
        { "platform_core", luaopen_seal_platform },
        { "texure_core", luaopen_seal_texture },
        { "sprite_core", luaopen_seal_sprite },
        
        { "socket.core", luaopen_socket_core },
        { "mime.core", luaopen_mime_core },
        { "cjson", luaopen_cjson },
        { "zlib", luaopen_zlib },

        { "nuklear_core", luaopen_nuklear_core },
#ifdef PLAT_DESKTOP
        { "nanovg_core", luaopen_nanovg_core },
#endif

        {NULL, NULL}
    };
    
    luaL_Reg* lib = lua_modules;
    lua_getglobal(L, "package");
    lua_getfield(L, -1, "preload");
    for (; lib->func; lib++) {
        lua_pushcfunction(L, lib->func);
        lua_setfield(L, -2, lib->name);
    }
    lua_pop(L, 2);
}
