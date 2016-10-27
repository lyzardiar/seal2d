#ifndef __seal__ttf_font__
#define __seal__ttf_font__

#include "platform/platform.h"

// TODO: add ttf support for mobile, load the libpng either.
#ifdef PLAT_DESKTOP

#include "ft2build.h"
#include FT_FREETYPE_H

struct texture;

struct ttf_font {
    const char* path;
    size_t font_size;
    
    FT_Face face;
    struct texture* tex;
};

void ttf_init_module();

struct ttf_font* ttf_font_new(const char* path, size_t font_size);
void ttf_font_free(struct ttf_font* font);

#endif

#endif