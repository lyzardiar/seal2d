#ifndef __seal__texture__
#define __seal__texture__

#include <OpenGL/gl3.h>

struct texture {
    GLuint id;
    unsigned int width, height;
} texture;

struct texture* texture_load_from_png(const char* file_path);

#endif
