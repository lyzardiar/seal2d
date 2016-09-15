#ifndef spine_render_h
#define spine_render_h

#include "render.h"

struct spine_render_context {
    struct vertex_buffer* buffer;

    struct render_batch batches[MAX_RENDER_BATCH];
    struct render_state state;

    int current_batch_index;
    int n_objects;
};

struct spine_render_vertex_desc {
    float* vertex;
    float* uv;
    float* color;
    GLuint tex_id;
};

void spine_render_batch_reset(struct render_batch* self);

void spine_render_func_init(struct render* R);
void spine_render_destroy(struct render* R);
void spine_render_func_start(struct render* R);
void spine_render_func_draw(struct render* R, void* object);
void spine_render_func_end(struct render* R);
void spine_render_func_flush(struct render* R);

#endif
