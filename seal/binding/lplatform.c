#include "../seal.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

int lplatform_write_s(lua_State* L)
{
    const char* path = luaL_checkstring(L, -1);
    const char* data = luaL_checkstring(L, -2);
    size_t size = fs_writes(path, data);
    if (size != strlen(data)) {
        luaL_error(L, "write failed, path = %s\n", path);
    }
    return 0;
}

int lplatform_read_s(lua_State* L)
{
    const char* path = luaL_checkstring(L, -1);
    const char* data = fs_reads(path);
    if (data) {
        lua_pushstring(L, data);
        s_free((void*)data);
        return 1;
    }
    return 0;
}

int lplatform_get_write_path(lua_State* L)
{
    lua_pushstring(L, fs_get_write_path());
    return 1;
}

int lplatform_get_sandbox_dir(lua_State* L)
{
    lua_pushstring(L, fs_sandbox_root_path());
    return 1;
}

int lplatform_get_platform(lua_State* L)
{
#ifdef PLAT_MAC
    lua_pushstring(L, "mac");
#elif PLAT_IOS
    lua_pushstring(L, "ios");
#elif PLAT_WIN
    lua_pushstring(L, "win");
#elif PLAT_ANDROID
    lua_pushstring(L, "android");
#endif
    return 1;
}


int lplatform_print_hook(lua_State* L)
{
    #if defined (SDK_DEBUG_LOG)
        // use #ifdef to prevent -wunsed
        const char    * msg     = lua_tostring(L, 1);
        LOGP_LUA("%s", msg);
    #endif

	return 0;
}


static int lplatform_lua_loader(lua_State* L)
{
    const char* name = luaL_checkstring(L, 1);

    char copy[64] = "";
    strncpy(copy, name, 64);
    char* dot = strstr(name, ".");
    // replace the dot to slash, seal.util to seal/util for instance.
    // the copied name is used for file loading.
    if(dot) {
        copy[dot - name] = '/';  //replace the dot with splash for file search
    }


    lua_getglobal(L, "package");
    lua_getfield(L, -1, "path");

    const char* search_path = lua_tostring(L, -1);
    char* p = NULL;
    char full_path[64] = "";
    char prefix[64] = "";
    size_t size = 0;
    // we were try to find the files in the package.path
    for (p = strtok((char*)search_path, ";"); p != NULL; p = strtok(NULL, ";"))
    {
        char* f = strstr(p, "?.lua");
        // we found the file we want
        if (f) {
            strncpy(prefix, p, f - p);
            snprintf(full_path, 64, "%s%s.lua", prefix, copy);
            LOGP("full_path is : %s\n", full_path);

            unsigned char* buff = fs_read(full_path, &size, 1);
            if (buff) {
                // load the file, name it `copy'
                luaL_loadbuffer(L, (const char*)buff, size, copy);
                return 0;
            }

            memset(full_path, 0, 64);
            memset(prefix, 0, 64);
        }
    }

#if defined PLAT_ANDROID
    // if no file found, we try to load it from the Assets.

    // WC - TODO
    return 0;
#endif
    return 1;
}


int lplatform_get_string_for_print(lua_State * L, char* out)
{
    int n = lua_gettop(L);  /* number of arguments */
    int i;

    lua_getglobal(L, "tostring");
    for (i=1; i<=n; i++) {
        const char *s;
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L, i);   /* value to print */
        lua_call(L, 1, 1);
        size_t sz;
        s = lua_tolstring(L, -1, &sz);  /* get result */
        if (s == NULL)
            return luaL_error(L, LUA_QL("tostring") " must return a string to "
                    LUA_QL("print"));
        if (i>1) strcat(out, "\t");
        strncat(out, s, sz);
        lua_pop(L, 1);  /* pop result */
    }
    return 0;
}

int lplatform_lua_print(lua_State * L)
{
    char buff[10240] = { 0 };
    lplatform_get_string_for_print(L, buff);
    LOGP_LUA("%s", buff);

    return 0;
}

int luaopen_seal_platform(lua_State* L)
{
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { "read_s", lplatform_read_s },
    #if defined (SEAL_USE_FSWRITE)
        { "write_s", lplatform_write_s },
        { "get_write_path", lplatform_get_write_path },
    #endif
        { "get_sandbox_root_path", lplatform_get_sandbox_dir },
        { "get_platform", lplatform_get_platform },
        { "__print", lplatform_lua_print },
        { "__loader", lplatform_lua_loader},
        { NULL, NULL },
    };

    luaL_newlib(L, lib);
    return 1;
}