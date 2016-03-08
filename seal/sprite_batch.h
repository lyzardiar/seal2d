#ifndef __seal__sprite_batch__
#define __seal__sprite_batch__

#include "math/geo.h"

// represent a sprite, minimize texture switch.
struct glyph {
    struct vertex tl;
    struct vertex bl;
    struct vertex tr;
    struct vertex br;

    GLuint tex_id;
    int draw_order;    
};

struct render_batch {
    GLuint offset;
    GLuint n_verts;
    GLuint tex_id;
};

struct sprite_batch {
    GLuint vbo;
    GLuint vao;
    struct array* glyphs;
    struct array* render_batches;
    
    int draw_order;
};

struct glyph* glyph_new();
void glyph_free(struct glyph* g);

struct sprite_batch* sprite_batch_new();
void sprite_batch_free(struct sprite_batch* batch);

void sprite_batch_begin(struct sprite_batch* self);
void sprite_batch_end(struct sprite_batch* self);

void sprite_batch_draw(struct sprite_batch* self,
                       const struct rect* dst_rect,
                       const struct rect* uv_rect,
                       const struct color* color,
                       const GLuint tex_id);

void sprite_batch_render(struct sprite_batch* self);
#endif
