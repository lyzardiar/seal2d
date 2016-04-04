#include "seal.h"

#include "memory.h"
#include "sprite_batch.h"
#include "texture.h"
#include "sprite.h"


EXTERN_GAME;

struct sprite* sprite_new(struct sprite_frame* frame){
    struct sprite* s = STRUCT_NEW(sprite);
    
    struct glyph* g = &s->glyph;
    struct rect* rect = &frame->rect;
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
    
    g->tex_id = frame->tex->id;
    s->frame = frame;
    return s;
}

void sprite_free(struct sprite* spr) {
    s_free(spr);
}

void sprite_visit(struct sprite* spr) {
    
}

void sprite_draw(struct sprite* self) {
    sprite_batch_draw(GAME->batch, &self->glyph, self->frame->tex->id);
}

void sprite_set_pos(struct sprite* self, float x, float y) {
    float w = self->frame->rect.width;
    float h = self->frame->rect.height;
    
    struct glyph* g = &self->glyph;
    SET_VERTEX_POS(g->bl, x, y);
    SET_VERTEX_POS(g->br, x + w, y);
    SET_VERTEX_POS(g->tl, x, y + h);
    SET_VERTEX_POS(g->tr, x + w, y + h);
}