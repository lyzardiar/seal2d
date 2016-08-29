#ifndef primitive_render_h
#define primitive_render_h

#include "render.h"

enum primitive_type {
    PRIMITIVE_INVALID = -1,
    PRIMITIVE_LINE = 0,
    PRIMITIVE_RECT,
    PRIMITVE_MAX,
};

struct primitive_render_batch {
    int offset;
    enum primitive_type type;
    int line_count;
    int line_width;
};

struct primitive_render_state {
    GLuint program;
    enum primitive_type last_type;
    struct attr_location loc;
    int line_width;
};

struct primitive_render_context {
    GLuint vao;
    GLuint vbo;
    float* vertexes;

    struct primitive_render_batch batches[MAX_RENDER_BATCH];
    struct primitive_render_state state;

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
