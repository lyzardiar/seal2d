#ifndef __seal__sprite__
#define __seal__sprite__

#include "base/array.h"

#include "math/matrix.h"
#include "math/geo.h"

#include "texture.h"
#include "sprite_batch.h"
#include "affine.h"

struct sprite_frame {
    struct texture* tex;
    
    struct rect rect;
    int ref;
};

struct sprite {
    // glphy information
    struct glyph glyph;
    
    struct affine local_srt;
    struct affine world_srt;
    
    int zorder;
    
    struct sprite* parent;
    struct array* children; // NULL indicates the children has been removed.
    size_t child_index;
    
    struct sprite_frame* frame;
    // privates
    int dirty;
};

struct sprite* sprite_alloc(struct sprite_frame* frame);
void sprite_free(struct sprite* spr);

void sprite_visit(struct sprite* self);
void sprite_draw(struct sprite* spr);

void sprite_set_pos(struct sprite* spr, float x, float y);
void sprite_set_rotation(struct sprite* spr, float rotation);
void sprite_set_scale(struct sprite* spr, float scale);

void sprite_add_child(struct sprite* self, struct sprite* child);

#endif