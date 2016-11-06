PROJECT(zlib)

cmake_minimum_required(VERSION 3.4.1)

INCLUDE_DIRECTORIES(
    ${ZLIB_DIR}
)

LINK_DIRECTORIES(${ZLIB_DIR})

SET(
    src_file
    tmp.c
)

ADD_LIBRARY(
    zlib STATIC ${src_file}
)

TARGET_LINK_LIBRARIES(zlib
   ${ZLIB_DIR}/libz.a
)


