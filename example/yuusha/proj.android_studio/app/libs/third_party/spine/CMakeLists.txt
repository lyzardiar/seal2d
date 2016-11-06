PROJECT(spine)

cmake_minimum_required(VERSION 3.4.1)

INCLUDE_DIRECTORIES(BEFORE
    ${SPINE_DIR}/include
)

FILE(GLOB src_files "${SPINE_DIR}/src/spine/*.c")

ADD_LIBRARY(spine STATIC ${src_files})