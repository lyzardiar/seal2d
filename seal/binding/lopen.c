#include "../seal.h"
#include "lua.h"
#include "luaconf.h"

// core engine part
extern int luaopen_seal_core(lua_State* L);
extern int luaopen_seal_platform(lua_State* L);
extern int luaopen_seal_texture(lua_State* L);
extern int luaopen_seal_sprite(lua_State* L);
extern int luaopen_seal_action(lua_State* L);

// third part part.
#if defined (SEAL_USE_LUASOCKET)
extern int luaopen_socket_core(lua_State *L);
extern int luaopen_mime_core(lua_State *L);
#endif
extern int luaopen_cjson(lua_State* L);
extern int luaopen_zlib(lua_State* L);

extern int luaopen_nuklear_core(lua_State* L);
extern int luaopen_nanovg_core(lua_State* L);

extern int lplatform_lua_print(lua_State* L);

void stackDump (lua_State *L)
{
    int i;
    int top = lua_gettop(L);
    for (i = 1; i <= top; i++) {  /* repeat for each level */
        int t = lua_type(L, i);
        switch (t) {

            case LUA_TSTRING:  /* strings */
                LOGP_LUA("`%s'", lua_tostring(L, i));
                break;

            case LUA_TBOOLEAN:  /* booleans */
                LOGP_LUA(lua_toboolean(L, i) ? "true" : "false");
                break;

            case LUA_TNUMBER:  /* numbers */
                LOGP_LUA("%g", lua_tonumber(L, i));
                break;

            default:  /* other values */
                LOGP_LUA("%s", lua_typename(L, t));
                break;

        }
        LOGP_LUA("    ");  /* put a separator */
    }
    LOGP_LUA("\n");  /* end the listing */
}

void setfiled_f(lua_State *L, const char* key, float f)
{
    lua_pushstring(L, key);
    lua_pushnumber(L, f);
    lua_settable(L, -3);
}

void setfiled_i(lua_State* L, const char* key, lua_Integer n)
{
    lua_pushstring(L, key);
    lua_pushinteger(L, n);
    lua_settable(L, -3);
}

void setfiled_s(lua_State *L, const char* key, const char* s)
{
    lua_pushstring(L, key);
    lua_pushstring(L, s);
    lua_settable(L, -3);
}

void setarray_n(lua_State* L, int index, lua_Number n)
{
    lua_pushinteger(L, index);
    lua_pushnumber(L, n);
    lua_settable(L, -3);
}

lua_Integer getfield_i(lua_State* L, const char* key)
{
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    lua_Integer value = lua_tointeger(L, -1);
    lua_pop(L, 1);
    return value;
}

lua_Number getfield_f(lua_State* L, const char* key)
{
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    lua_Integer value = lua_tonumber(L, -1);
    lua_pop(L, 1);
    return value;
}

const char* getfiled_s(lua_State* L, const char* key)
{
    lua_pushstring(L, key);
    lua_gettable(L, -2);
    const char* value = lua_tostring(L, -1);
    lua_pop(L, 1);
    return value;
}

void getarray_f(lua_State* L, const char* key, float* f_array, int cnt)
{
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

void getarray_i(lua_State* L, const char* key, int* f_array, int cnt)
{
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

void add_lua_loader(lua_State* L, lua_CFunction func)
{
    if (!func) return;

    // stack content after the invoking of the function
    // get loader table
    lua_getglobal(L, "package");                                  /* L: package */
    lua_getfield(L, -1, "loaders");                               /* L: package, loaders */

    if (lua_isnil(L, -1)){                                         /* L: package, nil */
        lua_pop(L, 1);                                              /* L: package */
        lua_newtable(L);                                            /* L: package table */
        lua_setfield(L, -2, "loaders");                            /* L: package */
        lua_getfield(L, -1, "loaders");                            /* L: package, loaders */
    }

    // insert loader into index 2
    lua_pushcfunction(L, func);
    int ll = lua_rawlen(L, -2);
    /* L: package, loaders, func */
    for (int i = (int)(ll + 1); i > 2; --i)
    {
        lua_rawgeti(L, -2, i - 1);                                /* L: package, loaders, func, function */
        // we call lua_rawgeti, so the loader table now is at -3
        lua_rawseti(L, -3, i);                                    /* L: package, loaders, func */
    }
    if (ll == 0){
        lua_rawseti(L, -2, 1);  /* L: package, loaders */
    }else {
        lua_rawseti(L, -2, 2);  /* L: package, loaders */
    }

    // set loaders into package
    lua_setfield(L, -2, "loaders");                               /* L: package */

    lua_pop(L, 1);
}

int luaopen_lua_loader(lua_State* L)
{
    const char* script_path = luaL_checkstring(L, -1);
    char* pptr = script_path;
    while (*pptr != '\0'){
        if (*pptr == '.'){
            *pptr = '/';
        }
        ++pptr;
    }
    char path [128] = {0};
    strcat(path, script_path);
    strcat(path, ".lua");

    size_t buff_len = 0;
    int ret = 1;
    const char* buff = fs_read(path, &buff_len, 0);
    if (luaL_loadbuffer(L, buff, buff_len, path)) {
        LOGP_LUA("load start script Failed. %s \n", lua_tostring(L, -1));
        ret = 0;
    }

    seal_free(buff);
    return ret;
}

void luaopen_lua_extensions(lua_State *L)
{
    // load extensions
    luaL_Reg lua_modules[] = {
        { "engine_core", luaopen_seal_core },
        { "platform_core", luaopen_seal_platform },
        { "texure_core", luaopen_seal_texture },
        { "sprite_core", luaopen_seal_sprite },
        { "action_core", luaopen_seal_action },

    #if defined (SEAL_USE_LUASOCKET)
        { "socket.core", luaopen_socket_core },
        { "mime.core", luaopen_mime_core },
    #endif
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

    lua_getglobal(L, "package");
    lua_pushcfunction(L, lplatform_lua_print);
    lua_setfield(L, -2, "print");
    lua_pushcfunction(L, luaopen_seal_platform);
    lua_setfield(L, -2, "open_platform");
    lua_pushcfunction(L, luaopen_lua_loader);
    lua_setfield(L, -2, "lua_loader");

    lua_pop(L, 1);

    lua_pop(L, 2);

    add_lua_loader(L, luaopen_lua_loader);
}
