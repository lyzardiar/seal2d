#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

#include "platform/fs.h"
#include "image/lodepng.h"

#include "util.h"
#include "shader.h"

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
    
    struct texture* tex = texture_load_from_mem(pixel_data, width, height, GL_RGBA);
 
    // TODO: third party use the standard malloc, we may replace that with s_malloc some day.
    
    free(pixel_data);
    return tex;
}

struct texture* texture_load_from_mem(const unsigned char* pixel,
                                      unsigned int width,
                                      unsigned int height,
                                      GLint mode) {
    // generate opengl texture
    struct texture* tex = s_malloc(sizeof(struct texture));
    tex->id = 0;
    glGenTextures(1, &tex->id);
    s_assert(tex->id != 0);
    
    glBindTexture(GL_TEXTURE_2D, tex->id);
    glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0, mode, GL_UNSIGNED_BYTE, pixel);
    CHECK_GL_ERROR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    CHECK_GL_ERROR
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    
    CHECK_GL_ERROR
    tex->width = width;
    tex->height = height;
    return tex;
}