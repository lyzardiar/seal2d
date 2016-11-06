PROJECT(third-party)

cmake_minimum_required(VERSION 3.4.1)

add_subdirectory(lua)
add_subdirectory(lua-cjson)
add_subdirectory(lua-socket)
add_subdirectory(lua-zlib)

add_subdirectory(freetype)
add_subdirectory(nanovg)
add_subdirectory(spine)
add_subdirectory(zlib)

SET(
    src_file
    tmp.c
)

ADD_LIBRARY(
    third_party STATIC ${src_file}
)

TARGET_LINK_LIBRARIES(third_party -Wl,-whole-archive lua -Wl,-no-whole-archive)
TARGET_LINK_LIBRARIES(third_party -Wl,-whole-archive lua-cjson -Wl,-no-whole-archive)
TARGET_LINK_LIBRARIES(third_party -Wl,-whole-archive lua-socket -Wl,-no-whole-archive)
TARGET_LINK_LIBRARIES(third_party -Wl,-whole-archive lua-zlib -Wl,-no-whole-archive)
TARGET_LINK_LIBRARIES(third_party
    freetype
    nanovg
    spine
    zlib
)


