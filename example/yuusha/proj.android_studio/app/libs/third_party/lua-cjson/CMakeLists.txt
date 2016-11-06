PROJECT(lua-cjson)

cmake_minimum_required(VERSION 3.4.1)

INCLUDE_DIRECTORIES(${LUA_DIR})

# ADD_DEFINITIONS(-DUSE_INTERNAL_FPCONV)

SET(src_files
    ${LUA_CJSON_DIR}/strbuf.c
    ${LUA_CJSON_DIR}/lua_cjson.c
    ${LUA_CJSON_DIR}/fpconv.c
)

ADD_LIBRARY(lua-cjson STATIC ${src_files})