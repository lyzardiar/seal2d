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


#ifndef __seal__fs__
#define __seal__fs__

#include "../seal_base.h"

// #include "platform.h"
// user's duty to free
// extra_byte: 0 or 1, if you were to read a string, we ensure another byte
// which is set '\0' to make a c-string.

const char* fs_full_path(const char* filename);
const char* fs_sandbox_root_path();

unsigned char* fs_read(const char* path, size_t* size, int extra_byte);
char* fs_reads(const char* path);

#if defined (SEAL_USE_FSWRITE)
size_t fs_writes(const char* path, const char* string);
size_t fs_writef(const char* path, const void* data, size_t size);

const char* fs_get_write_path();
#endif


#endif /* defined(__zore__zore_fs__) */
