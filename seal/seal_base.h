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


#ifndef __seal_base__
#define __seal_base__

// System libs and external dependencies, in ALPHABETICAL ordr

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

// WC: Hardcoding is preferred to keep cross-platform compatibilithy
// Different platforms has very different ways of specifying compiler flags such as -I
#include "../libs/lua-5.3.2/src/lua.h"
#include "../libs/lua-5.3.2/src/lauxlib.h"
#include "../libs/lua-zlib/unzip.h"
#include "../libs/nanovg/src/nanovg.h"
#include "../libs/spine/include/spine/spine.h"  // 这个项目的头文件用的是相对目录，加上-I ...

#if defined (__ANDROID__)
#include <android/log.h>
#include <android/asset_manager_jni.h>
#endif

#ifndef _WIN32
#include <unistd.h>
#include <sys/time.h>
#endif

#endif

