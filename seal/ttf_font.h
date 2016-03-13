#ifndef __seal__ttf_font__
#define __seal__ttf_font__

#include "ft2build.h"
#include FT_FREETYPE_H

struct ttf_font {
    const char* path;
    size_t font_size;
    
    FT_Face face;
};

void ttf_init_module();

struct ttf_font* ttf_font_new(const char* path, size_t font_size);
void ttf_font_free(struct ttf_font* font);


FT_Face ttf_load_face(const char* path);



#endif
