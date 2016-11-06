PROJECT(seal2d)

cmake_minimum_required(VERSION 3.4.1)

SET(CORE_DIR ${ROOT_DIR}/seal)

SET(LUA_DIR ${ROOT_DIR}/libs/lua-5.3.2/src)
SET(LUA_CJSON_DIR ${ROOT_DIR}/libs/lua-cjson)
SET(LUA_SOCKET_DIR ${ROOT_DIR}/libs/lua-socket)
SET(LUA_ZLIB_DIR ${ROOT_DIR}/libs/lua-zlib)

SET(FREETYPE_DIR ${PROJECT_SOURCE_DIR}/third_party/freetype/freetype2)
SET(NANOVG_DIR ${ROOT_DIR}/libs/nanovg/src)
SET(NUKLEAR_DIR ${ROOT_DIR}/libs/nuklear)

SET(SPINE_DIR ${ROOT_DIR}/libs/spine)
SET(ZLIB_DIR ${PROJECT_SOURCE_DIR}/third_party/zlib)


ADD_SUBDIRECTORY(
    ${PROJECT_SOURCE_DIR}/core
)
ADD_SUBDIRECTORY(
    ${PROJECT_SOURCE_DIR}/third_party
)

INCLUDE_DIRECTORIES(
    ${LUA_DIR}
)

SET(
    src_file
    tmp.cpp
)

ADD_LIBRARY(
    jni_test STATIC ${src_file}
)

ADD_LIBRARY(
    seal2d STATIC ${src_file}
)

TARGET_LINK_LIBRARIES(seal2d -Wl,-whole-archive jni_test -Wl,-no-whole-archive)
TARGET_LINK_LIBRARIES(seal2d
    core
    third_party
)