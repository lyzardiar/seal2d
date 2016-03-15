#ifndef __seal__texture__
#define __seal__texture__

#include <OpenGL/gl3.h>

struct texture {
    GLuint id;
    unsigned int width, height;
};

struct texture* texture_load_from_png(const char* file_path);

struct texture* texture_load_from_mem(const unsigned char* memory,
                                      unsigned int width,
                                      unsigned height, GLint mode);

#endif
