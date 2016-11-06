#include <jni.h>

#include "lua.hpp"

extern "C" {

    jstring Java_com_miyu_seal2d_seal2d_MainActivity_stringFromJNI(JNIEnv *env, jobject /* this */) {
        lua_State* L =  luaL_newstate();
        luaL_openlibs(L);
        luaL_dostring(L, " do return 'Call from Lua !!!' end ");
        const char* result = lua_tostring(L, -1);

        return env->NewStringUTF(result);
    }
}