#ifndef __seal__sprite__
#define __seal__sprite__
#include <stdbool.h>

#include "base/array.h"

#include "math/matrix.h"
#include "math/geo.h"

#include "texture.h"
#include "math/affine.h"

struct anim;
struct render;
struct touch_event;
struct array;
struct bmfont;

enum sprite_type {
    // single sprite
    SPRITE_TYPE_PIC = 0,
    SPRITE_TYPE_TTF_LABEL,
    SPRITE_TYPE_PARTICLE,
    SPRITE_TYPE_VECTOR, // contains line, rect, polygon
    
    // muti sprite
    SPRITE_TYPE_BMFONT_LABEL,
    SPRITE_TYPE_MESH,
    SPRITE_TYPE_TILE_MAP,
    
    // sprite container
    SPRITE_TYPE_CLIP,
    SPRITE_TYPE_CONTAINER,
};

struct sprite_frame {
    struct rect frame_rect;
    struct rect source_rect;
    struct size source_size;

    GLuint tex_id;
    
    bool rotated;
    bool trimmed;
    
    struct uv uv;
    
    char* key;
    
    // internal flag,
    bool __initialized;  // we have different procedure to init a sprite frame, so we have this flag
                         // to make life easier by not involving to many type of init functions.
};

struct sprite_frame_cache {
    struct Hashmap* cache;
    unsigned int nframes;
};

struct sprite_frame_cache* sprite_frame_cache_new();
void sprite_frame_cache_free(struct sprite_frame_cache* cache);
void sprite_frame_cache_add(struct sprite_frame_cache* self, struct sprite_frame* frame);
struct sprite_frame* sprite_frame_cache_get(struct sprite_frame_cache* self, const char* key);

struct sprite_frame* sprite_frame_new(const char* key);
void sprite_frame_free(struct sprite_frame* self);

void sprite_frame_set_texture_id(struct sprite_frame* self, GLuint tex_id);
void sprite_frame_init_uv(struct sprite_frame* self, float texture_width, float texture_height);
void sprite_frame_tostring(struct sprite_frame* self, char* buff);

struct sprite {
    // basic info
    unsigned int __id;
    enum sprite_type type;
    
    // scene graph
    int zorder;
    struct sprite* parent;
    struct array* children; // NULL indicates the children has been removed.
    size_t child_index;
    
    // geometry
    float x, y;
    float scale_x, scale_y;
    float rotation;
    float width, height;
    struct affine local_srt;
    struct affine world_srt;
    int dirty;
    
    // advanced
    color color;
    struct sprite_frame* frame;
    struct glyph glyph; // glphy information for rect sprites, this may waste some bytes. fix here someday.
    struct anim* anim; // for sprite's anim.
    struct bmfont* bmfont; // for sprite bmfont.
    
    
    bool swallow;
};

void sprite_init_render(struct render* render);

struct sprite* sprite_new(struct sprite_frame* frame);
struct sprite* sprite_new_label(const char* label);
struct sprite* sprite_new_bmfont_label(const char* label, const char* fnt_path);
struct sprite* sprite_new_container(struct rect* r);
struct sprite* sprite_new_clip(struct rect* r);
struct sprite* sprite_new_line(float* points);

void sprite_free(struct sprite* self);

void sprite_touch(struct sprite* self, struct touch_event* touch_event);
void sprite_visit(struct sprite* self, float dt);
bool sprite_contains(struct sprite* self, float x, float y);

void sprite_draw_pic(struct sprite* self);
void sprite_draw_label(struct sprite* self);
void sprite_draw_clip(struct sprite* self);
void sprite_clean_clip(struct sprite* self);

void sprite_set_sprite_frame(struct sprite* self, struct sprite_frame* frame);
void sprite_set_anim(struct sprite* self, struct anim* anim);

void sprite_set_pos(struct sprite* self, float x, float y);
void sprite_set_rotation(struct sprite* self, float rotation);
void sprite_set_scale(struct sprite* self, float scale);

void sprite_set_color(struct sprite* self, color color);

void sprite_add_child(struct sprite* self, struct sprite* child);
void sprite_remove_child(struct sprite* self, struct sprite* child);
void sprite_remove_all_child(struct sprite* self);

void sprite_to_node_space(struct sprite* self, float x, float y, float* tox, float* toy);


// !!! don't use this 2 methods unless you known what the HELL you were doing. :)
void sprite_init(struct sprite* self, float width, float height);
void sprite_set_glyph(struct sprite* self, struct rect* rect, struct uv* uv, GLuint tex_id);

#endif
