#ifndef __seal__sprite__
#define __seal__sprite__

#include "base/array.h"

#include "math/matrix.h"
#include "math/geo.h"

#include "texture.h"
#include "sprite_batch.h"
#include "affine.h"

typedef unsigned int sprite_frame_id;
struct sprite_frame {
    sprite_frame_id id;
    struct rect rect;
    unsigned int tex_id;
};

struct sprite_frame_cache {
    struct array* frames;
};

struct sprite_frame_cache* sprite_frame_cache_new();
void sprite_frame_cache_free(struct sprite_frame_cache* cache);

void sprite_frame_cache_load_from_json(const char* json);
void sprite_frame_cache_load_from_binary(const char* binary_file);
struct sprite_frame* sprite_frame_cache_get(sprite_frame_id id);
struct sprite_frame* sprite_frame_cache_get_with_name(const char* name);

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