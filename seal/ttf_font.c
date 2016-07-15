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

// still don't known why I can't render texture properly.
// I decide to use Bitmap font directly to render text for the time being.

#define USE_RGBA

struct ttf_font* ttf_font_new(const char* path, size_t font_size) {
    struct ttf_font* font = STRUCT_NEW(ttf_font);
    font->path = path;
    font->font_size = font_size;
    
    FT_Face face = ttf_load_face(path, font_size);
    font->face = face;
    
    // TODO: we need render this charactor.. Add TTF render to sprite
    FT_ULong uni_char = 0x67; // 'g'
    
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
    
    int target_w = face->size->metrics.max_advance >> 6;
    int target_h = face->size->metrics.height >> 6;
    
#ifdef  USE_RGBA
    int pixel_width = 4;
#else
    int pixel_width = 1;
#endif
    
    unsigned char* pixel = s_malloc(target_w * target_h * pixel_width);
    memset(pixel, 0, target_w * target_h * pixel_width);
    
    int asent = (int)(face->size->metrics.ascender >> 6);
    int offset_x = slot->bitmap_left;
    int offset_y = asent - slot->bitmap_top;
    printf("offset = (%d, %d), target size = (%d, %d)\n", offset_x, offset_y, target_w, target_h);

    unsigned int * dst = (unsigned int*)pixel;
    unsigned char* src = bitmap.buffer;
    
    printf("dst = (%p), src = (%p), \n", dst, src);
//    printf("----------------------------------------\n");
    for (int i = 0, p = offset_y; i < bitmap.rows; ++i, ++p) {
        for (int j = 0, q = offset_x; j < bitmap.width; ++j, ++q) {
#ifdef USE_RGBA
            dst[ p*target_w + q ] = (src[j]<<24) | (src[j]<<16) | (src[j]<<8) | (src[j]);
#else
            dst[ p*target_w + q ] = (src[j]);
#endif
//            printf("%c", src[j] != 0 ? '*' : ' ');
        }
//        printf("\n");
        src += bitmap.pitch;
    }
//    printf("----------------------------------------\n");
//
//    for (int i = 0; i < target_h; ++i) {
//        for (int j = 0; j < target_w; ++j) {
//            printf("%c", ((unsigned int*)pixel)[i*target_w + j] != 0 ? '*' : ' ');
//        }
//        printf("\n");
//    }
//    
#ifdef USE_RGBA
    struct texture* tex = texture_load_from_mem(pixel,
                                                target_w,
                                                target_h,
                                                GL_RGBA);
#else
    struct texture* tex = texture_load_from_mem(pixel,
                                                target_w,
                                                target_h,
                                                GL_RED);
#endif

    font->tex = tex;
    
    s_free(pixel);

    return font;
}

void ttf_font_free(struct ttf_font* font) {
    s_free(font);
}
