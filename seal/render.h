#ifndef __seal__render__
#define __seal__render__

#include "seal.h"

struct render {
    struct vertex** verts;
    unsigned int num_vertices;
};

void render_draw(struct render* r);

#endif /* defined(__yuusha__render__) */
