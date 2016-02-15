#ifndef __seal__vertex__
#define __seal__vertex__

#include "seal.h"

// 8 + 4 = 12bytes
typedef struct vertex {
    GLfloat position[2]; // x, y
    GLubyte color[4];   // r, g, b, a
    float uv[2];
} vertex;

#endif
