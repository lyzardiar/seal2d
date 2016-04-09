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

struct sprite* sprite_new(struct sprite_frame* frame){
    struct sprite* s = STRUCT_NEW(sprite);
    
    struct glyph* g = &s->glyph;
    struct rect* rect = &frame->source_rect;
    SET_VERTEX_POS(g->bl, 0.0f, 0.0f);
    SET_VERTEX_COLOR(g->bl, 1.0f, 1.0f, 1.0f, 1.0f);
    SET_VERTEX_UV(g->bl, 0.0f, 0.0f);
    
    SET_VERTEX_POS(g->br, rect->width, 0.0f);
    SET_VERTEX_COLOR(g->br, 1.0f, 1.0f, 1.0f, 1.0f);
    SET_VERTEX_UV(g->br, 1.0f, 0.0f);
    
    SET_VERTEX_POS(g->tl, 0.0f, rect->height);
    SET_VERTEX_COLOR(g->tl, 1.0f, 1.0f, 1.0f, 1.0f);
    SET_VERTEX_UV(g->tl, 0.0f, 1.0f);
    
    SET_VERTEX_POS(g->tr, rect->width, rect->height);
    SET_VERTEX_COLOR(g->tr, 1.0f, 1.0f, 1.0f, 1.0f);
    SET_VERTEX_UV(g->tr, 1.0f, 1.0f);
    
    g->tex_id = frame->tex_id;
    s->frame = frame;
    s->parent = NULL;
    s->dirty = 1;
    s->zorder = 0;
    
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
        struct affine tmp;
        af_identify(&tmp);
        
        // TODO: the root has no father, here we may write better code
        if (self->parent) {
            af_concat(&tmp, &(self->parent->local_srt));
        }
        af_concat(&tmp, &self->local_srt);
        self->world_srt = tmp;
        
        float x = self->world_srt.x;
        float y = self->world_srt.y;
        float w = self->frame->source_size.width;
        float h = self->frame->source_size.height;
    
        struct glyph* g = &self->glyph;
        SET_VERTEX_POS(g->bl, x, y);
        SET_VERTEX_POS(g->br, x + w, y);
        SET_VERTEX_POS(g->tl, x, y + h);
        SET_VERTEX_POS(g->tr, x + w, y + h);

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
    af_set_translate(&self->local_srt, x, y);
    
    self->dirty = 1;
}