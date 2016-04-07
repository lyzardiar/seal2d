#include <string.h>

#include "lopen.h"
#include "memory.h"

#include "platform/fs.h"

int lplatform_write_s(lua_State* L) {
    const char* path = luaL_checkstring(L, -1);
    const char* data = luaL_checkstring(L, -2);
    size_t size = s_writes(path, data);
    if (size != strlen(data)) {
        luaL_error(L, "write failed, path = %s\n", path);
    }
    return 0;
}

int lplatform_read_s(lua_State* L) {
    const char* path = luaL_checkstring(L, -1);
    const char* data = s_reads(path);
    if (data) {
        lua_pushstring(L, data);
        s_free((void*)data);
        return 1;
    }
    return 0;
}


int luaopen_seal_platform(lua_State* L) {
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { "read_s", lplatform_read_s },
        { "write_s", lplatform_write_s },
        { NULL, NULL },
    };
    
    luaL_newlib(L, lib);
    return 1;
}