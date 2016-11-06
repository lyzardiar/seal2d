PROJECT(lua-socket)

cmake_minimum_required(VERSION 3.4.1)

INCLUDE_DIRECTORIES(BEFORE
    ${LUA_DIR}
)

FILE(GLOB src_files "${LUA_DIR}/*.c")
LIST(REMOVE_ITEM src_files
    wsocket.c
)

ADD_LIBRARY(lua-socket STATIC ${src_files})