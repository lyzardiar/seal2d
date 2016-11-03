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

    #define PLAT_WIN (1)

	#ifndef M_PI
	#define M_PI       3.14159265358979323846
	#endif

#include <winsock2.h>

int gettimeofday(struct timeval * val, struct timezone *z);
int strcasecmp(char *s1, char *s2);
int strncasecmp(char *s1, char *s2, register int n);

#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
        #define PLAT_IOS (2)
        #define PLAT_IOS_SIM (3)

    #elif TARGET_OS_IPHONE
        #define PLAT_IOS (2)
        #define PLAT_IOS_DEVICE (4)

    #elif TARGET_OS_MAC
        #define PLAT_MAC (5)

    #else
    #   error "Unknown Apple platform"
    #endif
#elif __ANDROID__
    #define PLAT_ANDROID (6)

    // WC - 这个头文件不应该在这里。。。。
    // 最好每个platform一个头文件，比较干净
    #include <sys/time.h>

#elif __linux__
    #define PLAT_LINUX (7)

#elif __unix__
    #define PLAT_UNIX (8)

#elif defined(_POSIX_VERSION)
    #define PLAT_POSIX (9)

#else
#   error "Unknown compiler"
#endif

#if defined (PLAT_WIN) || defined (PLAT_MAC) || defined (PLAT_LINUX)
    #define PLAT_DESKTOP
#endif

#if defined (PLAT_IOS) || defined (PLAT_ANDROID)
    #define PLAT_MOBILE
#endif

#ifndef PLATFORM
    #ifdef PLAT_WIN
        #define PLATFORM PLAT_WIN
    #endif

    #ifdef PLAT_MAC
        #define PLATFORM PLAT_MAC
    #endif

    #ifdef PLAT_IOS
        #define PLATFORM PLAT_IOS
    #endif

    #ifdef PLAT_ANDROID
        #define PLATFORM PLAT_ANDROID
    #endif

    #ifdef PLAT_LINUX
        #define PLATFORM PLAT_LINUX
    #endif
#endif

#endif
