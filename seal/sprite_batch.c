#include "sprite_batch.h"

struct glyph* glyph_new() {
    return STRUCT_NEW(glyph);
}

void glyph_free(struct glyph* g) {
    s_free(g);
}

void sprite_batch_gen_vao(struct sprite_batch* self) {
    if(!self->vao) {
        glGenVertexArrays(1, &self->vao);
    }
    glBindVertexArray(self->vao);
    if(!self->vbo) {
        glGenBuffers(1, &self->vbo);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, self->vbo);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, VERTEX_OFFSET_POS);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, VERTEX_SIZE, VERTEX_OFFSET_COLOR);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, VERTEX_OFFSET_UV);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

struct sprite_batch* sprite_batch_new() {
    struct sprite_batch* batch = STRUCT_NEW(sprite_batch);
    
    batch->vao = 0;
    batch->vbo = 0;
    
    batch->glyphs = array_new(6*100);
    batch->render_batches = array_new(32);
    batch->draw_order = 0;
    
    sprite_batch_gen_vao(batch);
    return batch;
}

void sprite_batch_free(struct sprite_batch* batch) {
    s_free(batch);
}

void sprite_batch_begin(struct sprite_batch* self) {
    array_clear(self->glyphs, 1);
    array_clear(self->render_batches, 1);
}

// generate the render batch
void sprite_batch_gen_rb(struct sprite_batch* self) {
    size_t total_vertex = array_size(self->glyphs) * 6;
    struct vertex vertex[total_vertex];
    struct glyph* g0 = ((struct glyph*)array_at(self->glyphs, 0));
    
    struct render_batch* r = STRUCT_NEW(render_batch);
    int offset = 0;
    int cv = 0;
    vertex[cv++] = g0->tl;
    vertex[cv++] = g0->bl;
    vertex[cv++] = g0->br;
    vertex[cv++] = g0->br;
    vertex[cv++] = g0->tr;
    vertex[cv++] = g0->tl;
    offset += 6;
    
    r->offset = offset;
    r->tex_id = g0->tex_id;
    r->n_verts = 6;
    
    for (int i = 1; i < array_size(self->glyphs); ++i) {
        struct glyph* g = ((struct glyph*)array_at(self->glyphs, i));
        struct glyph* g_prev = ((struct glyph*)array_at(self->glyphs, i-1));
        if(g->tex_id != g_prev->tex_id) {
            struct render_batch* r =  STRUCT_NEW(render_batch);
            r->offset = offset;
            r->tex_id = g->tex_id;
            r->n_verts = 6;
            array_push_back(self->render_batches, r);
        } else {
            struct render_batch* r = array_at(self->render_batches,
                                              array_size(self->render_batches)-1);
            r->n_verts += 6;
        }
        offset += 6;
    }
    glBindBuffer(GL_ARRAY_BUFFER, self->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(struct vertex) * total_vertex, vertex, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int tex_compare(const void* a, const void* b) {
    struct glyph* ga = (struct glyph*)a;
    struct glyph* gb = (struct glyph*)b;
    if(ga->tex_id == gb->tex_id) {
        return ga->draw_order - gb->draw_order;
    } else {
        return ga->tex_id - gb->tex_id;
    }
}

void sprite_batch_end(struct sprite_batch* self) {
    qsort(array_data(self->glyphs), array_size(self->glyphs), sizeof(void*), tex_compare);
    
    sprite_batch_gen_rb(self);
}

void sprite_batch_draw(struct sprite_batch* self,
                       const struct rect* dst_rect,
                       const struct rect* uv_rect,
                       const struct color* color,
                       const GLuint tex_id) {
    struct glyph* g = glyph_new();
    g->tex_id = tex_id;
    
    // bottom left
    SET_VERTEX_POS(g->bl, dst_rect->x, dst_rect->y);
    SET_VERTEX_UV(g->bl, uv_rect->x, uv_rect->y);
    SET_VERTEX_COLOR(g->bl, color->r, color->g, color->b, color->a);
    
    // bottom right
    SET_VERTEX_POS(g->br, dst_rect->x + dst_rect->width, dst_rect->y);
    SET_VERTEX_UV(g->br, uv_rect->x + dst_rect->width, uv_rect->y);
    SET_VERTEX_COLOR(g->br, color->r, color->g, color->b, color->a);
    
    // top left
    SET_VERTEX_POS(g->tl, dst_rect->x, dst_rect->y + dst_rect->height);
    SET_VERTEX_UV(g->tl, uv_rect->x, uv_rect->y + dst_rect->height);
    SET_VERTEX_COLOR(g->tl, color->r, color->g, color->b, color->a);
    
    // top right
    SET_VERTEX_POS(g->tr, dst_rect->x + dst_rect->width, dst_rect->y + dst_rect->height);
    SET_VERTEX_UV(g->tr, uv_rect->x + dst_rect->width, uv_rect->y + dst_rect->height);
    SET_VERTEX_COLOR(g->tr, color->r, color->g, color->b, color->a);
    
    array_push_back(self->glyphs, g);
}