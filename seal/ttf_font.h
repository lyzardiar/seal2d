#ifndef __seal__ttf_font__
#define __seal__ttf_font__

#include "ft2build.h"
#include FT_FREETYPE_H

void ttf_init();
FT_Face ttf_load_face(const char* path);


#endif
