#include <stdio.h>

#include "platform/fs.h"
#include "util.h"
#include "memory.h"

#include "texture.h"

#include "ttf_font.h"

#include "image/lodepng.h"

static FT_Library library = NULL;

void ttf_init_module() {
    if (!library) {
        FT_Error err = FT_Init_FreeType(&library);
        if (err) {
            fprintf(stderr, "ttf_init init freetype library error: %d\n", err);
        }
    }
}

static FT_Face ttf_load_face(const char* path) {
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

unsigned char* draw_bitmap(FT_Bitmap* bitmap, int x, int y) {
    //TODO : read the document to figure out
    // the glyph metrics and rewrite this code.
    int i, j;
    int bw = bitmap->width;
    int bh = bitmap->rows;
    int w = x + bw;
    int h = y + bh;
#if 0

    int p, q;

    printf("(bw,bh) = (%d, %d), (w, h) = (%d, %d) \n", bw, bh, w, h);
    
    unsigned char* out = s_malloc(w * h);
    memset(out, 0, w*h);

    unsigned char* src = bitmap->buffer;
    for ( i = x, p = 0; i < w; i++, p++ ) {
        for ( j = y, q = 0; j < h; j++, q++ ) {
            out[j*w + i] = src[q * bw + p];
        }
    }
    return out;
#else

    unsigned char* buffer = s_malloc(w * h);
    unsigned char* out = buffer;
    memset(buffer,0, w * h);
    
    int offset = y * w + x;
    buffer += offset;
    
    unsigned char* src = bitmap->buffer;
    for (i = 0; i < bitmap->rows; ++i) {
        for (j = 0; j < bitmap->width; ++j) {
            buffer[j] = src[j];
        }
        buffer += w;
        src += bitmap->pitch;
    }

    return out;
#endif
}

struct ttf_font* ttf_font_new(const char* path, size_t font_size) {
    struct ttf_font* font = STRUCT_NEW(ttf_font);
    font->path = path;
    font->font_size = font_size;
    
    FT_Face face = ttf_load_face(path);
    font->face = face;
    
    FT_Set_Pixel_Sizes(face, 0, (unsigned int)font_size);
    
    // TODO: we need render this charactor.. Add TTF render to sprite
    FT_ULong uni_char = 0x61;
    
    FT_Error err = FT_Load_Char(face, uni_char, FT_LOAD_NO_BITMAP);
    if(err) {
        fprintf(stderr, "ttf_font_new load char error: %d\n", err);
        return NULL;
    }
    
    FT_GlyphSlot slot = face->glyph;
    err = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
    if (err) {
        fprintf(stderr, "ttf_font_new render glyph error: %d\n", err);
        return NULL;
    }
    
    FT_Bitmap bitmap = slot->bitmap;
    
    int asent = (int)(face->size->metrics.ascender >> 6);
    
    int ox = slot->bitmap_left;
    int oy = (int)(asent - slot->bitmap_top);
    
    unsigned char* buff = draw_bitmap(&slot->bitmap, ox, oy);
    
    struct texture* tex = texture_load_from_mem(buff,
                                                ox + bitmap.width,
                                                oy + bitmap.rows,
                                                GL_RED);

    font->tex = tex;
    
    s_free(buff);

    return font;
}

void ttf_font_free(struct ttf_font* font) {
    s_free(font);
}
