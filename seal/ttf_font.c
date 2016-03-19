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


#define WIDTH   640
#define HEIGHT  480


/* origin is the upper left corner */
unsigned char image[HEIGHT][WIDTH];


/* Replace this function with something useful. */

void
draw_bitmap2( FT_Bitmap*  bitmap,
            FT_Int      x,
            FT_Int      y)
{
    FT_Int  i, j, p, q;
    FT_Int  x_max = x + bitmap->width;
    FT_Int  y_max = y + bitmap->rows;
    
    
    for ( i = x, p = 0; i < x_max; i++, p++ )
    {
        for ( j = y, q = 0; j < y_max; j++, q++ )
        {
            if ( i < 0      || j < 0       ||
                i >= WIDTH || j >= HEIGHT )
                continue;
            
            image[j][i] |= bitmap->buffer[q * bitmap->width + p];
        }
    }
}


void
show_image( void )
{
    int  i, j;
    
    
    for ( i = 0; i < HEIGHT; i++ )
    {
        for ( j = 0; j < WIDTH; j++ )
            putchar( image[i][j] == 0 ? ' '
                    : image[i][j] < 128 ? '+'
                    : '*' );
        putchar( '\n' );
    }
}



unsigned char buffer[HEIGHT][WIDTH] = {0};

unsigned char*
draw_bitmap( FT_Bitmap*  bitmap,

            int dst_width,
            int dst_height,
            FT_Int      x,
            FT_Int      y)
{
    FT_Int  i, j, p, q;
    FT_Int  x_max = x + bitmap->width;
    FT_Int  y_max = y + bitmap->rows;
    
#if 0
    printf("x_max, y_max = %d, %d\n", x_max, y_max);
    
    int total = 0;
    for ( i = x, p = 0; i < x_max; i++, p++ )
    {
        for ( j = y, q = 0; j < y_max; j++, q++ )
        {
            if ( i < 0      || j < 0       ||
                i >= WIDTH || j >= HEIGHT )
                continue;

            ++total;
            printf("i = %d, j = %d, p = %d, q = %d\n", i,j,p,q);
            buffer[j][i] |= bitmap->buffer[q * bitmap->width + p];
        }
    }
    printf("total = %d\n", total);
    return buffer;
#else
    
    unsigned char* buffer = s_malloc(dst_width*dst_height);
    unsigned char* old = buffer;
    memset(buffer, 0, dst_height*dst_height);
    buffer += y * dst_width + x;
    unsigned char* src = bitmap->buffer;
    printf("src:");
    for (i = 0; i < bitmap->rows; ++i) {
        for (j = 0; j < bitmap->width; ++j) {
            printf("%d ",src[j]);
            buffer[j] = src[j];
        }
        buffer+= dst_width;
        src += bitmap->pitch;
    }
    
    printf("\n###########################\n\n");
    
    for (i = 0; i < dst_height; ++i) {
        for (j = 0; j < dst_width; ++j) {
            unsigned char data = old[i*dst_width + j];
//            printf("%d", data);
            putchar( data == 0 ? ' '
                    : data < 128 ? '+'
                    : '*' );
        }
        putchar('\n');
    }
    
    printf("\n###########################\n\n");
    return old;
    
#endif
}

struct ttf_font* ttf_font_new(const char* path, size_t font_size) {
    struct ttf_font* font = STRUCT_NEW(ttf_font);
    font->path = path;
    font->font_size = font_size;
    
    FT_Face face = ttf_load_face(path);
    font->face = face;
    
    FT_Set_Pixel_Sizes(face, 0, (unsigned int)font_size);
    
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
    
//    int pitch = bitmap.pitch;
    unsigned int width = 200; //bitmap.width;
    unsigned int height = 200; //bitmap.rows;
    
    printf("width, height = %d, %d\n", width, height);
    
    long asent = face->size->metrics.ascender >>6;
    long h = face->size->metrics.height >> 6;
    
    int ox = slot->bitmap_left;
    int oy = (int)(asent - slot->bitmap_top);
    
    printf("ox, oy = %d, %d\n", ox, oy);
    
    size_t ctx_h = face->size->metrics.height >> 6;
    size_t ctx_w = slot->advance.x >> 6;
    
    unsigned char* buff = draw_bitmap(&slot->bitmap,
                                      ctx_w,
                                      ctx_h,
                                      slot->bitmap_left,
                                      asent - slot->bitmap_top);
////    
//    draw_bitmap2(&slot->bitmap,
//                 slot->bitmap_left,
//                 HEIGHT - slot->bitmap_top);
//    
//    show_image();
//
//            struct texture* tex = texture_load_from_mem(buff, WIDTH, HEIGHT, GL_RED);
    struct texture* tex = texture_load_from_mem(buff, ctx_w, ctx_h, GL_RED);
//    s_free(buffer);
    font->tex = tex;

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