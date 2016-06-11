#include "seal.h"

#include "util.h"

#include "memory.h"
#include "sprite_batch.h"
#include "texture.h"
#include "sprite.h"

EXTERN_GAME;

struct sprite_frame* sprite_frame_new() {
    struct sprite_frame* f = STRUCT_NEW(sprite_frame);
    return f;
}

void sprite_frame_free(struct sprite_frame* self) {
    s_free(self);
}

void sprite_frame_set_texture_id(struct sprite_frame* self, GLuint tex_id) {
    self->tex_id = tex_id;
}

void sprite_frame_init_uv(struct sprite_frame* self, float texture_width, float texture_height) {
    struct rect* frame_rect = &self->frame_rect;
    self->uv.u = frame_rect->x / texture_width;
    self->uv.v = frame_rect->y / texture_height;
    self->uv.w = frame_rect->width / texture_width;
    self->uv.h = frame_rect->height / texture_height;
}

struct sprite* sprite_new(struct sprite_frame* frame){
    struct sprite* s = STRUCT_NEW(sprite);
    
    struct uv* uv = &frame->uv;
    struct glyph* g = &s->glyph;
    struct rect* rect = &frame->source_rect;
    SET_VERTEX_POS(g->bl, 0.0f, 0.0f);
    SET_VERTEX_COLOR(g->bl, 1.0f, 1.0f, 1.0f, 1.0f);

    
    SET_VERTEX_POS(g->br, rect->width, 0.0f);
    SET_VERTEX_COLOR(g->br, 1.0f, 1.0f, 1.0f, 1.0f);

    
    SET_VERTEX_POS(g->tl, 0.0f, rect->height);
    SET_VERTEX_COLOR(g->tl, 1.0f, 1.0f, 1.0f, 1.0f);

    
    SET_VERTEX_POS(g->tr, rect->width, rect->height);
    SET_VERTEX_COLOR(g->tr, 1.0f, 1.0f, 1.0f, 1.0f);

    
    SET_VERTEX_UV(g->bl, uv->u,         uv->v);
    SET_VERTEX_UV(g->br, uv->u + uv->w, uv->v);
    SET_VERTEX_UV(g->tl, uv->u,         uv->v + uv->h);
    SET_VERTEX_UV(g->tr, uv->u + uv->w, uv->v + uv->h);


    g->tex_id = frame->tex_id;
    s->frame = frame;
    s->parent = NULL;
    s->dirty = 1;
    s->zorder = 0;
    s->scale_x = s->scale_y = 1;
    s->rotation = 0;
    s->x = s->y = 0;
    
    s->children = array_new(16);
    
    af_identify(&s->local_srt);
    af_identify(&s->world_srt);
    return s;
}

void sprite_free(struct sprite* spr) {
    s_free(spr);
}

// update the coordinate from local to world
void sprite_update_transform(struct sprite* self) {
    if (self->dirty) {
        
        struct affine* local = &self->local_srt;
        af_srt(local, self->x, self->y, self->scale_x, self->scale_y, self->rotation);
        
        struct affine tmp;
        af_identify(&tmp);
        
        // TODO: the root has no father, here we may write better code
        if (self->parent) {
            af_concat(&tmp, &(self->parent->world_srt));
        }
        af_concat(&tmp, &self->local_srt);
        self->world_srt = tmp;
        
        float left = self->x;
        float right = self->x + self->frame->frame_rect.width;
        float bottom = self->y;
        float top = self->y + self->frame->frame_rect.height;
        
        //  p3--------p2
        //  |         |
        //  |         |
        //  |         |
        //  p0--------p1
        float x0 = left;
        float y0 = bottom;
        float x1 = right;
        float y1 = bottom;
        float x2 = left;
        float y2 = top;
        float x3 = right;
        float y3 = top;
        
        struct affine* world = &self->world_srt;
        af_mul(world, &x0, &y0);
        af_mul(world, &x1, &y1);
        af_mul(world, &x2, &y2);
        af_mul(world, &x3, &y3);
        
        struct glyph* g = &self->glyph;
        SET_VERTEX_POS(g->bl, x0, y0);
        SET_VERTEX_POS(g->br, x1, y1);
        SET_VERTEX_POS(g->tl, x2, y2);
        SET_VERTEX_POS(g->tr, x3, y3);
        
        self->dirty = 0;
    }
}

void sprite_add_child(struct sprite* self, struct sprite* child) {
    s_assert(child);
    child->child_index = array_size(self->children);
    
    // TODO: when we add the child, search the first NULL position.
    // TODO: consider the ZORDER
    array_push_back(self->children, child);
    child->parent = self;
}

void sprite_remove_child(struct sprite* self, struct sprite* child) {
    s_assert(child);
    array_set(self->children, child->child_index, NULL);
}

void sprite_remove_all_child(struct sprite* self) {
    
}

void sprite_visit(struct sprite* self) {
    struct array* children = self->children;
    for (int i = 0 ;i < array_size(children); ++i) {
        struct sprite* child = (struct sprite*)array_at(children, i);
        if (child) { // NULL indicates that the child has been removed
            
            // recursively visit the children.
            sprite_visit(child);
        }
    }
    
    sprite_update_transform(self);
    sprite_draw(self);
}

void sprite_draw(struct sprite* self) {
    sprite_batch_draw(GAME->batch, &self->glyph, self->frame->tex_id);
}

void sprite_set_pos(struct sprite* self, float x, float y) {
    self->x = x;
    self->y = y;
    
    self->dirty = 1;
}

void sprite_set_rotation(struct sprite* self, float rotation) {
    self->rotation = rotation;
    
    self->dirty = 1;
}
void sprite_set_scale(struct sprite* self, float scale) {
    self->scale_x = self->scale_y = scale;
    
    self->dirty = 1;
}

void sprite_run_action(struct sprite* self, struct action* action) {
    
}