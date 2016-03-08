#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

#include "platform/fs.h"
#include "image/lodepng.h"

#include "util.h"

#include "texture.h"

struct texture* texture_load_from_png(const char* file_path) {
    if(!file_path) {
        fprintf(stderr, "texure, texture_load_from_png, file_path is nil?");
        return NULL;
    }
    
    // load png data into memory
    size_t file_size = 0;
    unsigned char* origin_data = s_read(file_path, &file_size, 0);

    unsigned char* pixel_data = NULL;
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int error = 0;
    
    error = lodepng_decode32(&pixel_data, &width, &height, origin_data, file_size);
    s_free(origin_data);
    if(error) {
        fprintf(stderr, "error %u: %s\n", error, lodepng_error_text(error));
        return NULL;
    }
    
    // generate opengl texture
    struct texture* tex = s_malloc(sizeof(texture));
    tex->id = 0;
    glGenTextures(1, &tex->id);
    s_assert(tex->id != 0);
    
    glBindTexture(GL_TEXTURE_2D, tex->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    tex->width = width;
    tex->height = height;
    
    // TODO: third party use the standard malloc, we may replace that with s_malloc some day.
    free(pixel_data);
    return tex;
}