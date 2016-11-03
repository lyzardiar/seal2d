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


#ifndef __seal__texture__
#define __seal__texture__

#include "seal_base.h"

// WC - todo fix


#include "platform/render_opengl.h"

struct array_map;
struct texture;
struct Hashmap;

struct texture_cache {
    struct Hashmap* cache;
};

struct texture_cache* texture_cache_new();
void texture_cache_free(struct texture_cache* self);

struct texture* texture_cache_load(struct texture_cache* self, const char* key);
void texture_cache_unload(struct texture_cache* self, const char* key);

#define TEX_NAME_LEN (128)
struct texture {
    unsigned int ref;
    GLuint id;
    unsigned int width, height;

    int cursor_x, cursor_y;
    int row_height;
    char name[TEX_NAME_LEN];
};

struct texture* texture_load_from_png(const char* file_path);
struct texture* texture_load_from_bmp(const char* file_path);

struct texture* texture_load_from_mem(const unsigned char* memory,
                                      unsigned int width,
                                      unsigned int height, GLint mode);

void texture_set_row_height(struct texture* self, unsigned int row_height);

void texture_append(struct texture* self,
                    const unsigned char* memory,
                    unsigned int w,
                    unsigned int h,
                    GLint mode);

void texture_unload(struct texture* self);
//------------------------------------------------------------


#endif
