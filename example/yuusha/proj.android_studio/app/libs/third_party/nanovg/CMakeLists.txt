PROJECT(nanovg)

cmake_minimum_required(VERSION 3.4.1)

INCLUDE_DIRECTORIES(BEFORE
    ${LUA_DIR}
)

FILE(GLOB src_files "${NANOVG_DIR}/*.c")

ADD_LIBRARY(nanovg STATIC ${src_files})