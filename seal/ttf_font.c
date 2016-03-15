#include <stdio.h>

#include "platform/fs.h"
#include "util.h"
#include "memory.h"

#include "texture.h"

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


#define WIDTH   200
#define HEIGHT  200

unsigned char image[HEIGHT][WIDTH];


void
draw_bitmap_2( FT_Bitmap*  bitmap,
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

unsigned char*
draw_bitmap( FT_Bitmap*  bitmap,
            FT_Int      x,
            FT_Int      y)
{
    FT_Int  i, j, p, q;
    FT_Int  x_max = x + bitmap->width;
    FT_Int  y_max = y + bitmap->rows;
    
    unsigned char* buff = s_malloc(x_max * y_max); // rgba8888 = 4byte
    memset(buff, 0, x_max*y_max);
    
    for ( i = x, p = 0; i < x_max; i++, p++ )
    {
        for ( j = y, q = 0; j < y_max; j++, q++ )
        {
//            if ( i < 0      || j < 0       ||
//                i >= WIDTH || j >= HEIGHT )
//                continue;
          

            buff[ (j*x_max + i) ] |= bitmap->buffer[q * bitmap->width + p];;
        }
    }
    return buff;
}


void
show_image( void )
{
    int  i, j;
    
    
    for ( i = 0; i < HEIGHT; i++ )
    {
        for ( j = 0; j < WIDTH; j++ )
            putchar( image[i][j] == 0 ? ' '
                    : image[i][j] <= 128 ? ' '
                    : '*' );
        putchar( '\n' );
    }
}

struct ttf_font* ttf_font_new(const char* path, size_t font_size) {
    struct ttf_font* font = STRUCT_NEW(ttf_font);
    font->path = path;
    font->font_size = font_size;
    
    FT_Face face = ttf_load_face(path);
    font->face = face;
    
    FT_Set_Pixel_Sizes(face, 0, (unsigned int)font_size);
    
    FT_ULong uni_char = 0x61;
    
    FT_Error err = FT_Load_Char(face, uni_char, FT_LOAD_RENDER);
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
    

    draw_bitmap_2(&slot->bitmap,
                  slot->bitmap_left,
                  HEIGHT - slot->bitmap_top);
    show_image();
    
    FT_Bitmap bitmap = slot->bitmap;
    
//    int pitch = bitmap.pitch;
    unsigned int width = bitmap.width;
    unsigned int height = bitmap.rows;
    
    struct texture* tex = texture_load_from_mem(bitmap.buffer, width, height, GL_RGBA);
    
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