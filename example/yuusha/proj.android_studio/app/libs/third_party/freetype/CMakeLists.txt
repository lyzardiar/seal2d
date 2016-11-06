PROJECT(freetype)

cmake_minimum_required(VERSION 3.4.1)

INCLUDE_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/freetype2

)

LINK_DIRECTORIES(${PROJECT_SOURCE_DIR})

SET(
    src_file
    tmp.c
)

ADD_LIBRARY(
    freetype STATIC ${src_file}
)

TARGET_LINK_LIBRARIES(freetype
   ${PROJECT_SOURCE_DIR}/libfreetype-v7a.a
)


