#ifndef __seal__sprite__
#define __seal__sprite__

#include "base/array.h"

#include "math/matrix.h"
#include "math/geo.h"

#include "texture.h"
#include "sprite_batch.h"

struct sprite_frame {
    struct texture* tex;
    
    struct rect rect;
};

struct sprite {
    // glphy information
    struct glyph glyph;
    
    struct mat4 local_srt;
    struct mat4 world_srt;
    
    int zorder;
    
    struct array* children;
    
    struct sprite_frame* frame;
    // privates
    int dirty;
};

struct sprite* sprite_new(struct sprite_frame* frame);
void sprite_free(struct sprite* spr);

void sprite_draw(struct sprite* spr);

void sprite_set_pos(struct sprite* spr, float x, float y);
void sprite_set_rotation(struct sprite* spr, float rotation);
void sprite_set_scale(struct sprite* spr, float scale);



#endif