#ifndef spine_render_h
#define spine_render_h

#include "render.h"

struct spine_render_context {
    struct render_context __super;
    struct vertex_buffer* buffer;
};

struct spine_render_vertex_desc {
    float* vertex;
    float* uv;
    unsigned char* color;
    GLuint tex_id;
    float x, y;
};

void spine_render_batch_reset(struct render_batch* self);

void spine_render_func_init(struct render* R);
void spine_render_destroy(struct render* R);
void spine_render_func_start(struct render* R);
void spine_render_func_draw(struct render* R, void* object);
void spine_render_func_end(struct render* R);
void spine_render_func_flush(struct render* R);

#endif
