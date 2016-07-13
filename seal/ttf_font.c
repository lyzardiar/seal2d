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

static FT_Face ttf_load_face(const char* path, size_t font_size) {
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
    
    FT_Set_Pixel_Sizes(face, 0, (unsigned int)font_size);

    
#if DEBUG
    printf("face loaded: %s \n", path);
    printf("\tnum_faces = %ld\n", face->num_faces);
    printf("\tnum_glyphs = %ld\n", face->num_glyphs);
    printf("\tunits_per_EM = %d\n", face->units_per_EM);
    printf("\tnum_fixed_sizes = %d\n", face->num_fixed_sizes);
    for (int i = 0; i < face->num_fixed_sizes; ++i) {
        FT_Bitmap_Size size = face->available_sizes[i];
        printf("available_sizes[%d] = {width = %d, height = %d, size = %ld, x_ppem = %ld, y_ppem = %ld}\n",
               i, size.width, size.height, size.size, size.x_ppem, size.y_ppem );
    }
    
    
    FT_Size_Metrics metrics = face->size->metrics;

    printf("\tsize metrics = \n\t\t{\n\t\t\tx_ppem = %d,\n"
                             "\t\t\t y_ppem = %d,\n"
                             "\t\t\t x_scale = %ld,\n"
                             "\t\t\t y_scale = %ld,\n"
                             "\t\t\t ascender = %ld,\n"
                             "\t\t\t descender = %ld,\n"
                             "\t\t\t height = %ld,\n"
                             "\t\t\t max_advance = %ld\n\t\t}\n",
                                metrics.x_ppem, metrics.y_ppem, metrics.x_scale, metrics.y_scale,
                                metrics.ascender, metrics.descender, metrics.height, metrics.max_advance);

#endif
    
    s_free(ft_data);
    return face;
}

struct bitmap {
    void * buf;
    int w;
    int h;
    int pitch;
};

/* x,y is offset of target */
static void
cpy_bitmap(struct bitmap * target, struct bitmap * src, int x, int y) {
    unsigned char * sbuf = src->buf;
    unsigned char * tbuf = target->buf;
    
    int x0,y0,x1,y1;
    x0 = x > 0 ? x : 0;
    y0 = y > 0 ? y : 0;
    x1 = (x + src->w > target->w) ? target->w : (x+src->w);
    y1 = (y + src->h > target->h) ? target->h : (y+src->h);
    
    if (x1 <= x0 || y1 <= y0)
        return;
    if (x0 >= target->w || y0 >= target->h)
        return;
    
    int w = x1 - x0;
    int h = y1 - y0;
    
    tbuf += y0 * target->pitch + x0;
    sbuf += (y0 - y)*src->pitch + x0 - x;
    int i,j;
    for (i=0;i<h;i++) {
        for (j=0;j<w;j++)
            tbuf[j] = sbuf[j];
        sbuf += src->pitch;
        tbuf += target->pitch;
    }
}

//unsigned char* draw_bitmap(FT_Bitmap* bitmap, int ox, int oy, int dst_width, int dst_height) {
//    //TODO : read the document to figure out
//    // the glyph metrics and rewrite this code.
//    int bw = bitmap->width;
//    int bh = bitmap->rows;
//    int w = ox + bw;
//    int h = oy + bh;
//    printf("(bw,bh) = (%d, %d), (w, h) = (%d, %d), (ox, oy) = (%d, %d)\n", bw, bh, w, h, ox, oy);
//
//#if 1
//
//    unsigned char* out = s_malloc(dst_width * dst_height);
//    unsigned char* ret = out;
//    memset(out, 0, dst_width*dst_height);
//    
//    printf("offset = %d\n", oy * bw + ox);
//    unsigned char* src = bitmap->buffer;
//    src += dst_height - oy * bw + ox;
//
//    for (int i = 0; i < bh; ++i) {
//        for (int j = 0; j < bw; ++j) {
//            out[j] = src[j];
//        }
//        out += dst_width;
//        src += bw;
//    }
//
//    
//    printf("####################################\n");
//    for (int i = 0; i < dst_width; ++i) {
//        for (int j = 0; j < dst_height; ++j) {
//            printf("%c ", ret[j*h+i] ? '*' : ' ');
//        }
//        printf("\n");
//    }
//    printf("####################################\n");
//    
//    return ret;
//#else
//
//    unsigned char* buffer = s_malloc(w * h);
//    unsigned char* out = buffer;
//    memset(buffer, 0, w * h);
//    
//    int offset = oy * w + ox;
//    buffer += offset;
//    
//    printf("the origin buffer is\n");
//    printf("------------------------------------\n");
//    for (int i = 0; i < bitmap->rows; ++i) {
//        for (int j = 0; j < bitmap->width; ++j) {
//            printf("%c ", bitmap->buffer[i*bitmap->rows + j] != 0 ? '*' : ' ');
//        }
//        printf("\n");
//    }
//    printf("------------------------------------\n");
//    unsigned char* src = bitmap->buffer;
//    for (i = 0; i < bitmap->rows; ++i) {
//        for (j = 0; j < bitmap->width; ++j) {
//            buffer[j] = src[j];
//        }
//        buffer += w;
//        src += bitmap->pitch;
//    }
//
//    return out;
//#endif
//}

struct ttf_font* ttf_font_new(const char* path, size_t font_size) {
    struct ttf_font* font = STRUCT_NEW(ttf_font);
    font->path = path;
    font->font_size = font_size;
    
    FT_Face face = ttf_load_face(path, font_size);
    font->face = face;
    
    // TODO: we need render this charactor.. Add TTF render to sprite
    FT_ULong uni_char = 0x62;
    
    FT_Error err = FT_Load_Char(face, uni_char, FT_LOAD_NO_BITMAP);
    if(err) {
        fprintf(stderr, "ttf_font_new load char error: %d\n", err);
        return NULL;
    }
    
    FT_GlyphSlot slot = face->glyph;
    
    FT_Glyph_Metrics glyph_metrics = slot->metrics;
    
    err = FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);
    if (err) {
        fprintf(stderr, "ttf_font_new render glyph error: %d\n", err);
        return NULL;
    }
    
#if DEBUG
    printf("\n\tglyph metrics for char(\'%c\' - unicode[%lu]) = \n\t\t{\n\t\t\t width = %ld,\n"
           "\t\t\t height = %ld,\n"
           "\t\t\t horiBearingX = %ld,\n"
           "\t\t\t horiBearingY = %ld,\n"
           "\t\t\t horiAdvance = %ld,\n"
           "\t\t\t vertBearingX = %ld,\n"
           "\t\t\t vertBearingY = %ld,\n"
           "\t\t\t vertAdvance = %ld,\n\t\t}\n",
           (char)uni_char, uni_char, glyph_metrics.width, glyph_metrics.height, glyph_metrics.horiBearingX,
           glyph_metrics.horiBearingY, glyph_metrics.horiAdvance, glyph_metrics.vertBearingX,
           glyph_metrics.vertBearingY, glyph_metrics.vertAdvance);
    
    char format[5] = {0};
    format[0] = (face->glyph->format >> 24) & 0xff;
    format[1] = (face->glyph->format >> 16) & 0xff;
    format[2] = (face->glyph->format >> 8) & 0xff;
    format[3] = (face->glyph->format) & 0xff;
    printf("\t face glyph format = \'%s\'\n", format);
#endif
    
    FT_Bitmap bitmap = slot->bitmap;
    
    int asent = (int)(face->size->metrics.ascender >> 6);
    
    int ox = slot->bitmap_left;
    int oy = (int)(asent - slot->bitmap_top);

#if DEBUG
    printf("\t bitmap = \n\t\t{\n"
           "\t\t\t rows = %u\n"
           "\t\t\t width = %u\n"
           "\t\t\t pitch = %d\n"
           "\t\t\t num_grays = %d\n"
           "\t\t\t buffer = %p\n"
           "\t\t\t pixel_mode = %d\n\n\t\t}\n",
           bitmap.rows, bitmap.width, bitmap.pitch, bitmap.num_grays,
           bitmap.buffer, bitmap.pixel_mode );
    
#endif
    
    printf("ox oy = %d, %d\n", ox, oy);
    int dst_height = face->size->metrics.height >> 6;
    int dst_width = slot->advance.x >> 6;

    struct bitmap src;
    struct bitmap target;
    src.buf = bitmap.buffer;
    src.pitch = bitmap.pitch;
    src.w = bitmap.width;
    src.h = bitmap.rows;
    target.buf = s_malloc(asent*dst_width);
    target.w = target.pitch = dst_width;
    target.h = dst_height;
    
    
    cpy_bitmap(&target, &src, ox, oy);
    struct texture* tex = texture_load_from_mem(target.buf,
                                                target.w,
                                                target.h,
                                                GL_RED);

    font->tex = tex;
    
    s_free(target.buf);


    return font;
}

void ttf_font_free(struct ttf_font* font) {
    s_free(font);
}
