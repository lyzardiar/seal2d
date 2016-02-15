#include "texture.h"

struct texture* load_from_png(const char* file_path) {
    if(!file_path) {
        fprintf(stderr, "texure, load_from_png, file_path is nil?");
        return NULL;
    }
    
    // load png data into memory
    size_t file_size = 0;
    unsigned char* data = s_read(file_path, &file_size, 0);

    unsigned char* image = NULL;
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int error = 0;
    
    error = lodepng_decode32(&image, &width, &height, data, file_size);
    
//    for(int i = 0; i < width*height; ++i) {
//        printf("%x", data[i]);
//    }
//    printf("\n");
    s_free(data);
    if(error) {
        fprintf(stderr, "error %u: %s\n", error, lodepng_error_text(error));
        return NULL;
    }
    
    // generate opengl texture
    struct texture* tex = s_malloc(sizeof(texture));
    tex->id = 0;
    glGenTextures(1, &tex->id);
    assert(tex->id != 0);
    
    glBindTexture(GL_TEXTURE_2D, tex->id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);



//    glGenerateMipmap(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, 0);

//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
//    glBindTexture(GL_TEXTURE_2D, 0);

    tex->width = width;
    tex->height = height;
    
    // TODO: third party use the standard malloc, we may replace that with s_malloc some day.
    free(image);
    return tex;
}