#include <stdio.h>

#include "platform/fs.h"
#include "util.h"
#include "memory.h"

#include "ttf_font.h"

static FT_Library library = NULL;

void ttf_init_module() {
    if (!library) {
        FT_Error err = FT_Init_FreeType(&library);
        if (err) {
            fprintf(stderr, "ttf_init init freetype library error: %d\n", err);
        }
    }
}

struct ttf_font* ttf_font_new(const char* path, size_t font_size) {
    struct ttf_font* font = STRUCT_NEW(ttf_font);
    font->path = path;
    font->font_size = font_size;
    
    FT_Face face = ttf_load_face(path);
    font->face = face;
    
    FT_Set_Pixel_Sizes(face, 0, (unsigned int)font_size);
    
    FT_ULong uni_char = 0x0061;
    FT_UInt index = FT_Get_Char_Index(face, uni_char);
    
    if(index == 0) {
        fprintf(stderr, "load index failed, not fount index for char %ld\n", uni_char);
        return NULL;
    }
    
    FT_Error err = FT_Load_Glyph(font->face,
                                 index,
                                 FT_LOAD_DEFAULT|FT_LOAD_NO_BITMAP);
    
    if (err) {
        fprintf(stderr, "ttf_font_new load glyph error: %d\n", err);
        return NULL;
    }
    
    FT_GlyphSlot slot = face->glyph;
    err = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
    if (err) {
        fprintf(stderr, "ttf_font_new render glyph error: %d\n", err);
        return NULL;
    }
    
    FT_Bitmap bitmap = slot->bitmap;
//    for (int i = 0; i < bitmap.width; ++i) {
//        for (int j = 0; j < bitmap.)
//    }

    return font;
}

void ttf_font_free(struct ttf_font* font) {
    s_free(font);
}


FT_Face ttf_load_face(const char* path) {
    s_assert(library);
    
    size_t file_size = 0;
    unsigned char* ft_data = s_read(path, &file_size, 0);
    if(!ft_data) {
        fprintf(stderr, "load ttf file data failed, path = %s\n", path);
        return NULL;
    }
    FT_Face face = NULL;
    
    FT_Error err = FT_New_Memory_Face(library, ft_data, file_size, 0, &face);
    if(err) {
        fprintf(stderr, "load font face failed with error = %d\n", err);
        return NULL;
    }
    
    s_free(ft_data);
    return face;
}