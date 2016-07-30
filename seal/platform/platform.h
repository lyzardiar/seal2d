#ifndef __seal__platform__
#define __seal__platform__

#define STRINGFY(x) #x
#define VALUE(x) STRINGFY(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)

#ifdef _WIN32
    #define PLAT_WIN32

    #ifdef _WIN64
        #undef  PLAT_WIN32
        #define PLAT_WIN64
    #endif

    #define PLAT_WIN

#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
        #define PLAT_IOS
        #define PLAT_IOS_SIM

    #elif TARGET_OS_IPHONE
        #define PLAT_IOS
        #define PLAT_IOS_DEVICE

    #elif TARGET_OS_MAC
        #define PLAT_MAC

    #else
    #   error "Unknown Apple platform"
    #endif

#elif __linux__
    #define PLAT_LINUX

#elif __unix__
    #define PLAT_UNIX

#elif defined(_POSIX_VERSION)
    #define PLAT_POSIX

#else
#   error "Unknown compiler"
#endif

#if defined (PLAT_WIN) || defined (PLAT_MAC) || defined (PLAT_LINUX)
    #define PLAT_DESKTOP
#endif

#endif
