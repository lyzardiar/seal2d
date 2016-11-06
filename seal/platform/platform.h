/*
 * Copyright (C) 2016 Tang Yiyang
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See BELOW for details.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#ifndef __seal__platform__
#define __seal__platform__

// WC - should not really use __ANDROID__
#if defined (__ANDROID__)
#include "android_platform.h"
#endif


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

#if !defined(PLAT_ANDROID)
#if defined (SDK_DEBUG_LOG)
    #define LOGI(...)       ((void) sprintf (stderr, "SEAL2D: " __VA_ARGS__))
    #define LOGW(...)       ((void) sprintf (stderr, "SEAL2D: " __VA_ARGS__))
    #define LOGV(...)       ((void) sprintf (stderr, "SEAL2D: " __VA_ARGS__))
    #define LOGP(...)       ((void) sprintf (stderr, "SEAL2D: " __VA_ARGS__))
    #define LOGP_LUA(...)   ((void) sprintf (stderr, "SEAL2D(LUA): " __VA_ARGS__))
#else
    #define LOGI(...) ((void*)0)
    #define LOGW(...) ((void*)0)
    #define LOGV(...) ((void*)0)
    #define LOGP(...)       /**/
    #define LOGP_LUA(...)   /**/
#endif
#endif

#endif
