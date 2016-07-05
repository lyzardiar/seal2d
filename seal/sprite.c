#include <string.h>

#include "base/hashmap.h"
#include "seal.h"
#include "memory.h"

#include "sprite_batch.h"
#include "texture.h"
#include "sprite.h"
#include "anim.h"
#include "render.h"
#include "event.h"

#include "util.h"


EXTERN_GAME;

static unsigned int __sprite_id = 0;
static struct render* R = NULL;
void sprite_init_render(struct render* render) {
    R = render;
}

static int hash_str(void* key) {
    return hashmapHash(key, strlen(key));
}

static bool hash_equal(void* a, void* b) {
    return strcmp(a, b) == 0;
}

struct sprite_frame_cache* sprite_frame_cache_new() {
    struct sprite_frame_cache* c = STRUCT_NEW(sprite_frame_cache);
    c->cache = hashmapCreate(128, hash_str, hash_equal);
    c->nframes = 0;
    return c;
}

void sprite_frame_cache_free(struct sprite_frame_cache* self) {
    hashmapFree(self->cache);
    s_free(self);
}

void sprite_frame_cache_add(struct sprite_frame_cache* self, struct sprite_frame* frame) {
    hashmapPut(self->cache, frame->key, frame);
}

struct sprite_frame* sprite_frame_cache_get(struct sprite_frame_cache* self, const char* key) {
    return hashmapGet(self->cache, (void*)key);
}

struct sprite_frame* sprite_frame_new(const char* key) {
    struct sprite_frame* f = STRUCT_NEW(sprite_frame);
    memset(f, 0, sizeof(struct sprite_frame));
    int len = strlen(key);
    f->key = s_calloc(len+1);
    strcpy(f->key, key);
    
    sprite_frame_cache_add(GAME->sprite_frame_cache, f);
    return f;
}

void sprite_frame_free(struct sprite_frame* self) {
    s_free(self->key);
    s_free(self);
}

void sprite_frame_set_texture_id(struct sprite_frame* self, GLuint tex_id) {
    self->tex_id = tex_id;
}

void sprite_frame_init_uv(struct sprite_frame* self, float texture_width, float texture_height) {
    struct rect* frame_rect = &self->frame_rect;
    self->uv.u = frame_rect->x / texture_width;
    self->uv.v = 1.0f - (frame_rect->y + frame_rect->height) / texture_height; // left corner is (0, 0)
    self->uv.w = frame_rect->width / texture_width;
    self->uv.h = frame_rect->height / texture_height;
}

void sprite_frame_tostring(struct sprite_frame* self, char* buff) {
    
    sprintf(buff, "{key = \"%s\",\n"
            "frame_rect = {%d, %d, %d, %d},\n"
            "source_rect = {%d, %d, %d, %d},\n"
            "size = {%d, %d},\n"
            "text_id = %d,\n"
            "rotated = %s,\n"
            "trimmed = %s,\n"
            "uv = {%.2f, %.2f, %.2f, %.2f}}\n",
            self->key,
            self->frame_rect.x, self->frame_rect.y, self->frame_rect.width, self->frame_rect.height,
            self->source_rect.x, self->source_rect.y, self->source_rect.width, self->source_rect.height,
            self->source_size.width, self->source_size.height,
            self->tex_id,
            stringfy_bool(self->rotated),
            stringfy_bool(self->trimmed),
            self->uv.u, self->uv.v, self->uv.w, self->uv.h);
}

static void sprite_init(struct sprite* self, float width, float height) {
    self->__id = ++__sprite_id;
    self->frame = NULL;
    self->parent = NULL;
    self->dirty = 1;
    self->zorder = 0;
    self->scale_x = self->scale_y = 1;
    self->rotation = 0;
    self->x = self->y = 0;
    self->width = width;
    self->height = height;
    self->anim = NULL;
    self->swallow = true;
    
    self->children = array_new(16);
    
    af_identify(&self->local_srt);
    af_identify(&self->world_srt);
}

static void sprite_set_glyph(struct sprite* self, struct rect* rect, struct uv* uv, GLuint tex_id) {
    struct glyph* g = &self->glyph;
    
    SET_VERTEX_POS(g->bl, 0.0f, 0.0f);
    SET_VERTEX_COLOR(g->bl, 1.0f, 1.0f, 1.0f, 1.0f);
    
    SET_VERTEX_POS(g->br, rect->width, 0.0f);
    SET_VERTEX_COLOR(g->br, 1.0f, 1.0f, 1.0f, 1.0f);
    
    SET_VERTEX_POS(g->tl, 0.0f, rect->height);
    SET_VERTEX_COLOR(g->tl, 1.0f, 1.0f, 1.0f, 1.0f);
    
    SET_VERTEX_POS(g->tr, rect->width, rect->height);
    SET_VERTEX_COLOR(g->tr, 1.0f, 1.0f, 1.0f, 1.0f);
    
    if (uv) {
        SET_VERTEX_UV(g->bl, uv->u,         uv->v);
        SET_VERTEX_UV(g->br, uv->u + uv->w, uv->v);
        SET_VERTEX_UV(g->tl, uv->u,         uv->v + uv->h);
        SET_VERTEX_UV(g->tr, uv->u + uv->w, uv->v + uv->h);
    }
    
    g->tex_id = tex_id;
}

struct sprite* sprite_new(struct sprite_frame* frame){
    struct sprite* s = STRUCT_NEW(sprite);
    s->type = SPRITE_TYPE_PIC;
    
    sprite_init(s, frame->source_rect.width, frame->source_rect.height);
    sprite_set_sprite_frame(s, frame);
    return s;
}

struct sprite* sprite_new_container(struct rect* r) {
    struct sprite* s = STRUCT_NEW(sprite);
    s->type = SPRITE_TYPE_CONTAINER;
    
    sprite_init(s, r->width, r->height);
    sprite_set_glyph(s, r, NULL, 0);
    return s;
}

struct sprite* sprite_new_clip(struct rect* r) {
    struct sprite* s = STRUCT_NEW(sprite);
    s->type = SPRITE_TYPE_CLIP;
    
    sprite_init(s, r->width, r->height);
    sprite_set_glyph(s, r, NULL, 0);
    
    s->x = r->x;
    s->y = r->y;
    return s;
}

void sprite_free(struct sprite* self) {
    if(self->anim) {
        anim_free(self->anim);
    }
    
    s_free(self);
}

// update the coordinate from local to world
void sprite_update_transform(struct sprite* self) {
    if (self->dirty) {
        for (int i = 0; i < array_size(self->children); ++i) {
            struct sprite* child = (struct sprite*)array_at(self->children, i);
            child->dirty = 1;
        }
        
        struct affine* local = &self->local_srt;
        af_srt(local, self->x, self->y, self->scale_x, self->scale_y, self->rotation);
        
        struct affine tmp;
        af_identify(&tmp);
        af_concat(&tmp, &self->local_srt);
        
        if (self->parent) {
            af_concat(&tmp, &(self->parent->world_srt));
        }
        
        float left = tmp.x;
        float right = tmp.x + self->width;
        float bottom = tmp.y;
        float top = tmp.y + self->height;
        
        struct glyph* g = &self->glyph;
        SET_VERTEX_POS(g->bl, left, bottom);
        SET_VERTEX_POS(g->br, right, bottom);
        SET_VERTEX_POS(g->tl, left, top);
        SET_VERTEX_POS(g->tr, right, top);
        
        self->dirty = 0;
        
        self->world_srt = tmp;
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

void sprite_touch(struct sprite* self, struct touch_event* touch_event) {
    struct array* children = self->children;
    for (int i = 0 ;i < array_size(children); ++i) {
        struct sprite* child = (struct sprite*)array_at(children, i);
        if (child) { // NULL indicates that the child has been removed
            
            // recursively visit the children.
            sprite_touch(child, touch_event);
        }
    }
    
    if(touch_event->swallowd) {
        return;
    }
    
    if (sprite_contains(self, touch_event->x, touch_event->y)) {
        if (self->swallow) {
            touch_event->swallowd = true;
        }
    }
}

bool sprite_contains(struct sprite* self, float x, float y) {
    struct rect world = {
        self->glyph.bl.position[0],
        self->glyph.bl.position[1],
        self->width,
        self->height,
    };
    return rect_contains(&world, x, y);
}

void sprite_visit(struct sprite* self, float dt) {
    if (self->anim) {
        anim_update(self->anim, dt);
        sprite_set_sprite_frame(self, anim_current_frame(self->anim));
    }
    
    sprite_update_transform(self);
    
    switch (self->type) {
        case SPRITE_TYPE_PIC:
            sprite_draw_pic(self);
            break;
        case SPRITE_TYPE_CONTAINER:
            // do nothing.
            break;
        case SPRITE_TYPE_CLIP:
            sprite_draw_clip(self);
            break;
        default:
            break;
    }
    
    struct array* children = self->children;
    for (int i = 0 ;i < array_size(children); ++i) {
        struct sprite* child = (struct sprite*)array_at(children, i);
        if (child) { // NULL indicates that the child has been removed
            
            // recursively visit the children.
            sprite_visit(child, dt);
        }
    }
    
    switch (self->type) {
        case SPRITE_TYPE_CLIP:
            sprite_clean_clip(self);
            break;
            
        default:
            break;
    }
}

void sprite_draw_pic(struct sprite* self) {
    render_use_shader(R, SHADER_COLOR);
    render_use_texture(R, self->frame->tex_id);
    render_buffer_append(R, &self->glyph);
}

void sprite_draw_clip(struct sprite* self) {
    struct rect r = {self->world_srt.x, self->world_srt.y, self->width, self->height};
    render_set_scissors(R, &r);
}

void sprite_clean_clip(struct sprite* self) {
    render_clear_scissors(R);
}

void sprite_set_sprite_frame(struct sprite* self, struct sprite_frame* frame) {
    sprite_set_glyph(self, &frame->source_rect, &frame->uv, frame->tex_id);
    self->frame = frame;
    self->dirty = 1;
}

void sprite_set_anim(struct sprite* self, struct anim* anim) {
    if (self->anim != anim) {
        if(self->anim) {
            anim_free(self->anim);
        }
        
        self->anim = anim;
        anim_play(anim);
    }
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
