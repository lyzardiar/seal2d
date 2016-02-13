#ifndef __yuusha__vertex__
#define __yuusha__vertex__

#include <OpenGL/gl.h>
typedef struct vertex {
    float position[2]; // x, y
    GLuint color[4];   // r, g, b, a
} vertex;

#endif
