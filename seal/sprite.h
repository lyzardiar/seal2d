#ifndef __seal__sprite__
#define __seal__sprite__
#include <stdbool.h>

#include "base/array.h"

#include "math/matrix.h"
#include "math/geo.h"

#include "texture.h"
#include "affine.h"

#define FILL_OUTLINE (1)
#define FILL_SOLID   (1)

struct anim;
struct render;
struct touch_event;
struct array;
struct bmfont;
struct spine_anim;

enum sprite_type {
    // single sprite
    SPRITE_TYPE_PIC = 0,
    SPRITE_TYPE_TTF_LABEL,
    SPRITE_TYPE_PRIMITVE, // contains line, rect, polygon, bezier line

    // muti sprite
    SPRITE_TYPE_SPINE,
    SPRITE_TYPE_BMFONT_LABEL,
    SPRITE_TYPE_MESH,
    SPRITE_TYPE_TILE_MAP,
    SPRITE_TYPE_PARTICLE,

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
    float anchor_x, anchor_y;
    struct affine local_srt;
    struct affine world_srt;
    unsigned int dirty;

    // advanced
    color color;
    struct sprite_frame* frame;
    struct glyph glyph; // glphy information for rect sprites, this may waste some bytes.
    struct anim* anim; // for sprite's anim.

    // for primitive
    int primitive_type;
    struct primitive_vertex* primitive_vertex;

    // for primitive-line
    int line_width;

    // for primitive-rect
    unsigned int rect_flag;
    color fill_color;
    color outline_color;

    // for sprite bmfont.
    struct bmfont* bmfont;
    char* text;

    // for spine.
    struct spine_anim* spine_anim;

    bool swallow;
    bool visible;
};

void sprite_init_render(struct render* render);

struct sprite* sprite_new(struct sprite_frame* frame);
struct sprite* sprite_new_label(const char* label);
struct sprite* sprite_new_bmfont_label(const char* label, const char* fnt_path);
struct sprite* sprite_new_container(struct rect* r);
struct sprite* sprite_new_spine(const char* atlas_path, const char* spine_data_path, float scale);
struct sprite* sprite_new_clip(struct rect* r);
struct sprite* sprite_new_line(float* vertex, float width, color line_color);
struct sprite* sprite_new_rect(struct rect* r, unsigned int rect_flag, color fill_color, color outline_color);

void sprite_free(struct sprite* self);

void sprite_touch(struct sprite* self, struct touch_event* touch_event);
void sprite_visit(struct sprite* self, float dt);
bool sprite_contains(struct sprite* self, float x, float y);

void sprite_set_sprite_frame(struct sprite* self, struct sprite_frame* frame);
void sprite_set_anim(struct sprite* self, struct anim* anim);
void sprite_set_spine_anim(struct sprite* self, const char* anim_name, int track, bool loop);

// common
void sprite_set_visible(struct sprite* self, bool visible);
void sprite_set_pos(struct sprite* self, float x, float y);
void sprite_set_anchor(struct sprite* self, float x, float y);
void sprite_set_rotation(struct sprite* self, float rotation);
void sprite_set_scale(struct sprite* self, float scale);

void sprite_set_color(struct sprite* self, color color);
void sprite_set_size(struct sprite* self, float width, float height);

void sprite_add_child(struct sprite* self, struct sprite* child);
void sprite_remove_child(struct sprite* self, struct sprite* child);
void sprite_remove_all_child(struct sprite* self);

void sprite_to_node_space(struct sprite* self, float x, float y, float* tox, float* toy);

// for label
void sprite_set_text(struct sprite* self, const char* label);

// !!! don't use this 2 methods unless you known what the you were doing. :)
void sprite_init(struct sprite* self, float width, float height);
void sprite_set_glyph(struct sprite* self, struct rect* rect, struct uv* uv, GLuint tex_id);

struct glyph* sprite_get_glyph(struct sprite* self);

#endif
