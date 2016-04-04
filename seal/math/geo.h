#ifndef __seal__geo__
#define __seal__geo__

#include <OpenGL/OpenGL.h>

struct vertex {
    GLfloat position[2]; // x, y
    GLubyte color[4];   // r, g, b, a
    float uv[2];        // u, v
};

#define SET_VERTEX_POS(vert, x, y) \
do { \
vert.position[0] = x; \
vert.position[1] = y; \
} while(0);

#define SET_VERTEX_COLOR(vert, r, g, b, a) \
do { \
vert.color[0] = r; \
vert.color[1] = g; \
vert.color[2] = b; \
vert.color[3] = a; \
} while(0);

#define SET_VERTEX_UV(vert, u, v) \
do { \
vert.uv[0] = u; \
vert.uv[1] = v; \
} while(0);

#define VERTEX_SIZE (sizeof(struct vertex))
#define VERTEX_OFFSET_POS ((void*)offsetof(struct vertex, position))
#define VERTEX_OFFSET_COLOR ((void*)offsetof(struct vertex, color))
#define VERTEX_OFFSET_UV ((void*)offsetof(struct vertex, uv))

struct color {
    GLubyte r;
    GLubyte g;
    GLubyte b;
    GLubyte a;
};

struct rect {
    int x, y;
    int width, height;
};

#endif /* defined(__yuusha__geo__) */
