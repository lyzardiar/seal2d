#ifndef sprite_render_h
#define sprite_render_h

#include "render.h"

struct sprite_render_context {
    struct vertex_buffer* buffer;
    struct render_batch batches[MAX_RENDER_BATCH];
    struct render_state state;

    int current_batch_index;
    int n_objects;
};

void sprite_render_func_init(struct render* R);
void sprite_render_func_start(struct render* R);
void sprite_render_func_flush(struct render* R);
void sprite_render_batch_reset(struct render_batch* self);
void sprite_render_func_draw(struct render* R, void* object);
void sprite_render_func_flush(struct render* R);

#endif /* sprite_render_h */
