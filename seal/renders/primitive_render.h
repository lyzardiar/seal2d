#ifndef primitive_render_h
#define primitive_render_h

#include "render.h"

enum primitive_type {
    PRIMITIVE_INVALID = -1,
    PRIMITIVE_LINE = 0,
    PRIMITIVE_RECT,
    PRIMITVE_MAX,
};

enum primitive_batch_type {
    PRIMITIVE_BATCH_LINE = 0,
    PRIMITIVE_BATCH_TRIANGLE,
};

struct primitive_render_batch {
    enum primitive_batch_type type;
    int offset;
    union {
        int line_count;
        int triangle_count;
    } count;
};

struct primitive_render_state {
    GLuint program;
    struct attr_location loc;
};

struct primitive_render_context {
    GLuint vao;
    GLuint vbo;
    struct primitive_vertex* vertexes;

    struct primitive_render_batch batches[MAX_RENDER_BATCH];
    struct primitive_render_state state;
    struct primitive_render_batch* current_batch;

    int current_batch_index;
    int offset;
    int n_objects;
};

void primitive_render_func_init(struct render* R);
void primitive_render_func_start(struct render* R);
void primitive_render_func_flush(struct render* R);
void primitive_render_func_end(struct render* R);

void primitive_render_func_draw(struct render* R, enum primitive_type primitive_type, void* object);

#endif
