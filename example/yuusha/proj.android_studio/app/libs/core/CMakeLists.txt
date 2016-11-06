PROJECT(core)

cmake_minimum_required(VERSION 3.4.1)

INCLUDE_DIRECTORIES(
    ${LUA_DIR}
    ${LUA_CJSON_DIR}
    ${LUA_SOCKET_DIR}
    ${LUA_ZLIB_DIR}
    ${FREETYPE_DIR}
    ${NANOVG_DIR}
    ${NUKLEAR_DIR}
    ${ZLIB_DIR}
    ${SPINE_DIR}/include

    ${CORE_DIR}
    ${CORE_DIR}/base
    ${CORE_DIR}/binding
    ${CORE_DIR}/image
    ${CORE_DIR}/math
    ${CORE_DIR}/platform
    ${CORE_DIR}/platform/android
    ${CORE_DIR}/renders
    ${CORE_DIR}/test
)

SET(
    src_base_file
    ${CORE_DIR}/base/array.c
    ${CORE_DIR}/base/hashmap.c
)

SET(
    src_binding_file
    ${CORE_DIR}/binding/laction.c
    ${CORE_DIR}/binding/lnanovg.c
    ${CORE_DIR}/binding/lnuklear.c
    ${CORE_DIR}/binding/lopen.c
    ${CORE_DIR}/binding/lplatform.c
    ${CORE_DIR}/binding/lseal_core.c
    ${CORE_DIR}/binding/lsprite.c
    ${CORE_DIR}/binding/ltexture.c
)

SET(
    src_image_file
    ${CORE_DIR}/image/lodepng.c
)

SET(
    src_math_file
    ${CORE_DIR}/math/affine.c
    ${CORE_DIR}/math/geo.c
    ${CORE_DIR}/math/matrix.c
)

SET(
    src_platform_file
    ${CORE_DIR}/platform/platform.c
    ${CORE_DIR}/platform/timer.c
    ${CORE_DIR}/platform/android/fs.c
)

SET(
    seal_help_jni_src

    ${CORE_DIR}/platform/android/jni/seal_help_jni.c
)

ADD_LIBRARY(
    seal_help_jni STATIC ${seal_help_jni_src}
)

SET(
    src_renders_file
    ${CORE_DIR}/renders/primitive_render.c
    ${CORE_DIR}/renders/spine_render.c
    ${CORE_DIR}/renders/sprite_render.c
)

SET(
    src_core_file

    ${CORE_DIR}/action.c
    ${CORE_DIR}/anim.c
    ${CORE_DIR}/bmfont.c
    ${CORE_DIR}/camera.c
    ${CORE_DIR}/event.c
    ${CORE_DIR}/lua_handler.c
    ${CORE_DIR}/memory.c
    ${CORE_DIR}/render.c
    ${CORE_DIR}/scheduler.c
    ${CORE_DIR}/seal.c
    ${CORE_DIR}/shader.c
    ${CORE_DIR}/spine_anim.c
    ${CORE_DIR}/sprite.c
    ${CORE_DIR}/texture.c
    ${CORE_DIR}/ttf_font.c
    ${CORE_DIR}/window.c
)

SET(
    src_file

    ${src_core_file}

    ${src_base_file}
    ${src_binding_file}
    ${src_image_file}
    ${src_math_file}
    ${src_platform_file}
    ${src_renders_file}

)

ADD_LIBRARY(
    core STATIC ${src_file}
)

TARGET_LINK_LIBRARIES(core -Wl,-whole-archive seal_help_jni -Wl,-no-whole-archive)


