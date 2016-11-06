PROJECT(Engine)

cmake_minimum_required(VERSION 3.4.1)

# MESSAGE(WARNING "ANDROID_NDK_ABI_NAME:" ${ANDROID_NDK_ABI_NAME})
# MESSAGE(WARNING "PROJECT_SOURCE_DIR:" ${PROJECT_SOURCE_DIR})

MACRO(android_ndk_import_module_cpufeatures)
    INCLUDE_DIRECTORIES(${ANDROID_NDK}/sources/android/cpufeatures)
    ADD_LIBRARY(cpufeatures ${ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c)
    TARGET_LINK_LIBRARIES(cpufeatures dl)
ENDMACRO()

SET(CMAKE_INCLUDE_DIRECTORIES_BEFORE ON)
SET(ROOT_DIR "${PROJECT_SOURCE_DIR}/../../../..")

ADD_DEFINITIONS(-DSDK_DEBUG_LOG)
ADD_DEFINITIONS(-D__ANDROID__)
ADD_DEFINITIONS(-DSEAL_USE_FSWRITE)

INCLUDE_DIRECTORIES(
    ${ROOT_DIR}/seal
    ${ROOT_DIR}/seal/platform
    ${ROOT_DIR}/libs/spine/include
)

ADD_SUBDIRECTORY(
    ${PROJECT_SOURCE_DIR}/libs
)

SET(
    src_file

    android_native_app_glue.c
    android_native_gl_helper.c
)

ADD_LIBRARY(
    engine_helper STATIC ${src_file}
)

ADD_LIBRARY(
    engine
    SHARED
    main.c
)

android_ndk_import_module_cpufeatures()

TARGET_LINK_LIBRARIES(engine -Wl,-whole-archive engine_helper -Wl,-no-whole-archive)
TARGET_LINK_LIBRARIES(engine

    seal2d
    m
    cpufeatures
    GLESv2
    EGL
    log
    android
)

SET_TARGET_PROPERTIES(engine
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_NDK_ABI_NAME}"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/src/main/libs/${ANDROID_NDK_ABI_NAME}"
)