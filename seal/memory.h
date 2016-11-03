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


#ifndef __seal__memory__
#define __seal__memory__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if DEBUG
// memory functions
#define s_malloc(size)          seal_malloc(size, __FILE__, __LINE__)
#define s_calloc(size)          seal_calloc(size, __FILE__, __LINE__)
#define s_realloc(p, size)      seal_realloc(p, size, __FILE__, __LINE__)
#define s_free(p)               seal_free((p))

#else

#define s_malloc    malloc
#define s_calloc    calloc
#define s_realloc   realloc
#define s_free      free

#endif


#define STRUCT_NEW(type) (struct type*)s_malloc(sizeof(struct type))

void add_entry(void* ptr, size_t size, const char* file, int line);
void delete_entry(void* ptr);

extern void* seal_malloc(size_t size, const char* file, int line);
extern void* seal_calloc(size_t size, const char* file, int line);
extern void* seal_realloc(void* ptr, size_t size, const char* file, int line);
extern void seal_free(void* ptr);
extern void seal_dump_memory();

#endif
