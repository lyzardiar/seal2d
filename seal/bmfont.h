#ifndef bmfont_h
#define bmfont_h

#include <stdio.h>

struct array;

#define BM_FACE_NAME_LEN    (128)
#define CHARSET_NAME_LEN    (32)
#define PAGE_FILE_NAME_LEN  (128)

// move outside of bmfont definition to get rid of warning. :0
struct charc {
    int id;
    int x, y, width, height;
    int xoffset, yoffset;
    int xadvance;
    int page; // should be 0 all the time, cause we only support 1 page right now.
    int chnl; // ?? what's this is used for
    char letter[4]; // for implemention simplicty, use more bytes :), for bytes is enough for most cases.
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
    
    struct array* characters;
};

struct bmfont* bmfont_new(const char* bmfont_data);
void bmfont_free(struct bmfont* self);

#endif
