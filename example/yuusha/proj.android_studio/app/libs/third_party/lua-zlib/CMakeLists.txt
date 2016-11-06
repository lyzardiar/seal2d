PROJECT(lua-zlib)

cmake_minimum_required(VERSION 3.4.1)

INCLUDE_DIRECTORIES(BEFORE
    ${LUA_DIR}
)

FILE(GLOB src_files "${LUA_ZLIB_DIR}/*.c")

ADD_DEFINITIONS(-DIOAPI_NO_64)

ADD_LIBRARY(lua-zlib STATIC ${src_files})