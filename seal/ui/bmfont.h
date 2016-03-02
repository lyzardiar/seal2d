#ifndef __seal__bmfont__
#define __seal__bmfont__

#include "seal.h"

struct charactor {
    int id;
    int x;
    int y;
    int width;
    int height;
    int xoffset;
    int yoffset;
    int xadvance;
    int page;
    int chnl;
    const char* letter;
};

struct kernel {
    int first;
    int second;
    int amount;
};

struct bmfont_info {
    // info part.
    const char* face;
    int size;
    int bold;
    int italic;
    const char* charset;
    int unicode;
    int stretchH;
    int smooth;
    int aa;
    int padding[4];
    int spacing[2];
    
    // common part.
    int lineHeight;
    int base;
    int scaleW;
    int scaleH;
    int pages;
    int packed;
    
    // page part
    int page_id; // id in file line 3.
    const char* file;
    
    // char part, dynamic allocated
    int char_count;
    struct charactor* chars;
    
    // kernel part
    int kernel_count;
    struct kernel* kernels;
};

struct bmfont_info* bmf_load_info_plain(const char* atlas_file_name,
                                        const char* fnt_file_name);

struct bmfont_info* bmf_load_info_binary(const char* atlas_file_name,
                                         const char* fnt_file_name);

#endif
