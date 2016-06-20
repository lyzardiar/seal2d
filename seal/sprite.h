#ifndef __seal__sprite__
#define __seal__sprite__

#include "platform/types.h"

#include "base/array.h"

#include "math/matrix.h"
#include "math/geo.h"

#include "texture.h"
#include "sprite_batch.h"
#include "affine.h"

struct anim;

enum sprite_type {
    SPRITE_TYPE_PIC = 0,
    SPRITE_TYPE_CONTAINER = 1,
};

struct sprite_frame {
    struct rect frame_rect;
    struct rect source_rect;
    struct size source_size;

    GLuint tex_id;
    
    bool rotated;
    bool trimmed;
    
    struct uv uv;
};

struct sprite_frame* sprite_frame_new();
void sprite_frame_free(struct sprite_frame* self);

void sprite_frame_set_texture_id(struct sprite_frame* self, GLuint tex_id);
void sprite_frame_init_uv(struct sprite_frame* self, float texture_width, float texture_height);

struct sprite {
    // glphy information
    struct glyph glyph;
    
    struct affine local_srt;
    struct affine world_srt;
    
    int zorder;
    
    struct sprite* parent;
    struct array* children; // NULL indicates the children has been removed.
    size_t child_index;
    
    float x, y;
    float scale_x, scale_y;
    float rotation;
    float width, height;
    
    int dirty;
    
    enum sprite_type type;
    
    // for sprite only
    struct sprite_frame* frame;
};


struct sprite* sprite_new(struct sprite_frame* frame);
struct sprite* sprite_new_container(struct rect* r);
void sprite_free(struct sprite* spr);

void sprite_visit(struct sprite* self);
void sprite_draw_pic(struct sprite* self);

void sprite_set_anim(struct sprite* self, struct anim* anim);

void sprite_set_pos(struct sprite* self, float x, float y);
void sprite_set_rotation(struct sprite* self, float rotation);
void sprite_set_scale(struct sprite* self, float scale);

void sprite_add_child(struct sprite* self, struct sprite* child);

void sprite_run_action(struct sprite* self, struct action* action);

#endif
