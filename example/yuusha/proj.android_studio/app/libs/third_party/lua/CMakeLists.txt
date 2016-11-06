PROJECT(lua)

cmake_minimum_required(VERSION 3.4.1)

INCLUDE_DIRECTORIES(BEFORE
    ${LUA_DIR}
)

FILE(GLOB src_files "${LUA_DIR}/*.c")
LIST(REMOVE_ITEM src_files
    lua.c
    luac.c
)

ADD_DEFINITIONS(-DLUA_USE_C89)

ADD_LIBRARY(lua STATIC ${src_files})