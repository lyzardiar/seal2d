//#include "lua.h"
//#include "lualib.h"
//#include "lauxlib.h"
//
//#include "sprite.h"
//
//
//int lsprite_alloc(lua_State* L) {
//    //TODO: memory management.
//    const char* frame_name = lua_tostring(L, -1);
//    
//    
//    return 1;
//}
//
//
//int luaopen_affine_core(lua_State* L) {
//    
//#ifdef luaL_checkversion
//    luaL_checkversion(L);
//#endif
//    luaL_Reg lib[] = {
//        { "alloc", lsprite_alloc },
//        { "free"  },
//        { "set_pos" },
//        { "set_scale"},
//        { "set_zorder"},
//        { "add_child" },
//        { NULL, NULL },
//    };
//    
//    luaL_newlib(L, lib);
//    
//    
//    return 1;
//}