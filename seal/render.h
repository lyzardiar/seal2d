#ifndef __yuusha__render__
#define __yuusha__render__

#include "seal.h"
#include "vertex.h"

struct render {
    vertex** verts;
    unsigned int num_vertices;
};

void render_draw(struct render* r);

#endif /* defined(__yuusha__render__) */
