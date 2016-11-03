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


#ifndef bmfont_h
#define bmfont_h

#include "seal_base.h"

#ifdef WIN32
#include <stdint.h>
#endif

struct Hashmap;
struct sprite;

#define BM_FACE_NAME_LEN    (128)
#define CHARSET_NAME_LEN    (32)
#define PAGE_FILE_NAME_LEN  (128)

// move outside of bmfont definition to get rid of warning. :0
struct charc {
    int64_t id;     // 64 (8bytes should be enough for most cases)
    int x, y, width, height;
    int xoffset, yoffset;
    int xadvance;
    int page; // should be 0 all the time, cause we only support 1 page right now.
    int chnl; // ?? what's this is used for
    char letter[8]; // for implemention simplicty, use more bytes :), 4 bytes is enough for most cases.
};

struct bmfont{
    struct info {
/* we only need the spacing right now */

//        char face[BM_FACE_NAME_LEN];
//        size_t size;
//        int bold;
//        int italic;
//        char charset[CHARSET_NAME_LEN];
//        int unicode;
//        int stretchH;
//        int smooth;
//        int aa;
//        int padding[4];
        struct spacing {
            int x;
            int y;
        } spacing;
    } info;

    struct common {
        int lineHeight;
        int base;
        int scaleW;
        int scaleH;
        int pages;
        int packed;
    } common;

    struct page {
        int id;
        char file[PAGE_FILE_NAME_LEN];
    }page;

    char fnt_file[PAGE_FILE_NAME_LEN];
    struct Hashmap* characters;
};


struct bmfont* bmfont_new(const char* bmfont_data);
void bmfont_free(struct bmfont* self);

struct charc* bmfont_load_charc(struct bmfont* self, const char* c);


struct bmfont_cache {
    struct Hashmap* cache;
    unsigned int nframes;
};

struct bmfont_cache* bmfont_cache_new();
void bmfont_cache_free(struct bmfont_cache* cache);
void bmfont_cache_add(struct bmfont_cache* self, struct bmfont* font, const char* key);
struct bmfont* bmfont_cache_get(struct bmfont_cache* self, const char* key);
#endif
