#include <string.h>

#include "platform/fs.h"

#include "base/hashmap.h"
#include "seal.h"
#include "memory.h"

#include "texture.h"
#include "ttf_font.h"
#include "sprite.h"
#include "anim.h"
#include "bmfont.h"
#include "render.h"
#include "event.h"

#include "lua_handler.h"

#include "util.h"


EXTERN_GAME;

// dirty flags
#define SPRITE_TRANSFORM_DIRTY  (1 << 0)
#define SPRITE_SCALE_DIRTY      (1 << 1)
#define SPRITE_ROTATION_DIRTY   (1 << 2)
#define SPRITE_COLOR_DIRTY      (1 << 3)
#define SPRITE_FRAME_DIRTY      (1 << 4)
#define SPRITE_ALL_DIRTY        ((unsigned int)-1)

#define SPRITE_SRT_DIRTY        (SPRITE_TRANSFORM_DIRTY | SPRITE_SCALE_DIRTY | SPRITE_ROTATION_DIRTY)

static unsigned int __sprite_id = 0;
static struct render* R = NULL;
static struct sprite_frame_cache* C = NULL;
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
    
    C = c;
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
    struct sprite_frame* f = hashmapGet(self->cache, (void*)key);
    if (!f) {
        f = sprite_frame_new(key);
        sprite_frame_cache_add(GAME->sprite_frame_cache, f);
    }
    return f;
}

struct sprite_frame* sprite_frame_new(const char* key) {
     struct sprite_frame* f = STRUCT_NEW(sprite_frame);
    memset(f, 0, sizeof(struct sprite_frame));
    
    int len = strlen(key);
    f->key = s_calloc(1, len+1);
    strcpy(f->key, key);
    
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

void sprite_init(struct sprite* self, float width, float height) {
    self->__id = ++__sprite_id;
    self->frame = NULL;
    self->parent = NULL;
    self->dirty = SPRITE_ALL_DIRTY;
    self->zorder = 0;
    self->scale_x = self->scale_y = 1;
    self->rotation = 0;
    self->x = self->y = 0;
    self->width = width;
    self->height = height;
    self->anim = NULL;
    self->bmfont = NULL;
    self->swallow = true;
    self->color = C4B_COLOR(255, 255, 255, 255);
    
    self->children = array_new(16);
    
    af_identify(&self->local_srt);
    af_identify(&self->world_srt);
}

void sprite_set_glyph(struct sprite* self, struct rect* rect, struct uv* uv, GLuint tex_id) {
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

struct sprite* sprite_new_label(const char* label) {
    struct sprite* s = STRUCT_NEW(sprite);
    s->type = SPRITE_TYPE_TTF_LABEL;
    
    // we new a frame
    struct sprite_frame* frame = sprite_frame_new(label);
    frame->source_rect.x = 0;
    frame->source_rect.y = 0;
    frame->source_rect.width = GAME->font->tex->width;
    frame->source_rect.height = GAME->font->tex->height;
    frame->tex_id = GAME->font->tex->id;
    
    frame->uv.u = 0.0f;
    frame->uv.v = 0.0f;
    frame->uv.w = 1.0f;
    frame->uv.h = 1.0f;
    
    sprite_init(s, GAME->font->tex->width, GAME->font->tex->height);
    sprite_set_sprite_frame(s, frame);

    return s;
}

struct sprite* sprite_new_bmfont_label(const char* label, const char* fnt_path) {
    struct rect r = {0, 0, 200, 200};
    struct sprite* root = sprite_new_container(&r);
    root->type = SPRITE_TYPE_BMFONT_LABEL;
    
    char* bmfont_data = s_reads(fnt_path);
    
    // TODO: we need cache the font to avoid load everytime.
    struct bmfont* bmfont = bmfont_new(bmfont_data);
    s_free(bmfont_data);
    
    /*for simpler implemention, each character will use a sprite,
      but will only cost one draw call at most. :)*/
//    sprite_init(root, 0, 0);
    
    // load the texture
    
    // get the path and append before page.file
    
    char* p = strrchr(fnt_path, '/');
    char tmp[128] = "";
    strncpy(tmp, fnt_path, p - fnt_path);
    char pagefile[128] = "";
    snprintf(pagefile, 128, "%s/%s", tmp, bmfont->page.file);
    struct texture* tex = texture_cache_load(GAME->texture_cache, pagefile);
    
    // not support chinese yet.
    char label_key[128] = "";
    int len = strlen(label);
    float x = 0.0f;
    float y = 0.0f;
    
    float width = 500;
    
    for (int i = 0; i < len; ++i) {
        
        // TODO: currently we only support ** ENGLISH **, we need have a UTF-8 character split function
        // to split this label into characters, so that we were able to support Chinese, Japanese..
        char c = label[i];
        snprintf(label_key, 128, "%s_%c", fnt_path, c);
        
        char str[4] = {c, 0, 0, 0};
        
        struct charc* character = bmfont_load_charc(bmfont, str);
        
        struct sprite_frame* frame = sprite_frame_cache_get(C, label_key);
        if (!frame->__initialized) {
            
            frame->frame_rect.x = character->x;
            frame->frame_rect.y = character->y;
            frame->frame_rect.width = character->width;
            frame->frame_rect.height = character->height;
            
            frame->source_rect = frame->frame_rect;
            
            frame->source_size.width = character->width;
            frame->source_size.height = character->height;
            
            frame->tex_id = tex->id;
            
            sprite_frame_init_uv(frame, tex->width, tex->height);
            frame->__initialized = true;
        }
        
        struct sprite* c_sprite = sprite_new(frame);
        sprite_set_pos(c_sprite, x, y);
        sprite_add_child(root, c_sprite);
        
        // coord caculation
        x += character->xoffset + character->xadvance;
        if (x > width) {
            x = 0;
            y -= bmfont->common.lineHeight;
        }
    }
    
    s_free(bmfont);
    return root;
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
    
    
    s_free(self->bmfont);
    
    s_free(self);
}

// update the coordinate from local to world
void sprite_update_transform(struct sprite* self) {
    
    // pass the dirty flags to the children
    for (int i = 0; i < array_size(self->children); ++i) {
        struct sprite* child = (struct sprite*)array_at(self->children, i);
        if (child) {
            child->dirty |= self->dirty;
        }
    }
    
    // TODO: we could improve performance by seprating transform from SRT,
    // but for simpler implemention, we have use SRT_DIRTY right now. :)
    if (self->dirty & SPRITE_SRT_DIRTY ) {
        
        struct affine* local = &self->local_srt;
        af_srt(local, self->x, self->y, self->scale_x, self->scale_y, self->rotation);
        
        struct affine tmp;
        af_identify(&tmp);
        af_concat(&tmp, &self->local_srt);
        
        if (self->parent) {
            af_concat(&tmp, &(self->parent->world_srt));
        }
        
        float left =    tmp.x;
        float right = tmp.x + self->width;
        float bottom = tmp.y;
        float top = tmp.y + self->height;
        
        struct glyph* g = &self->glyph;
        SET_VERTEX_POS(g->bl, left, bottom);
        SET_VERTEX_POS(g->br, right, bottom);
        SET_VERTEX_POS(g->tl, left, top);
        SET_VERTEX_POS(g->tr, right, top);
        
        self->dirty &= ~(SPRITE_SRT_DIRTY);
        
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
    
    // here we should release the memory??? yes.
    if (child) {
        sprite_free(child);
        // we only remove the child, but we don't move the array
        // TODO: move the array after set the element to NULL :)
        array_set(self->children, child->child_index, NULL);
    }
}

void sprite_remove_all_child(struct sprite* self) {
    struct array* children = self->children;
    for (int i = 0 ;i < array_size(children); ++i) {
        sprite_remove_child(self, array_at(children, i));
    }
}

// TODO: we should add a covert space function
void sprite_to_node_space(struct sprite* self, float x, float y, float* tox, float* toy) {
    
}

static int touch_event_set_func(lua_State* L, void* ud) {
    struct touch_event* event = (struct touch_event*)ud;
    lua_pushstring(L, SPRITE_EVENT_TYPE);
    lua_pushinteger(L, event->type);
    lua_pushinteger(L, event->x);
    lua_pushinteger(L, event->y);
    return 4;
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
        
        //TODO: this call is ugly, refactor someday.
        lua_handler_exe_func(GAME->lua_handler, GAME->lstate, self, touch_event_set_func, touch_event);
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
        case SPRITE_TYPE_TTF_LABEL:
            sprite_draw_pic(self);
//            sprite_draw_label(self);
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

void sprite_draw_label(struct sprite* self) {
    render_use_shader(R, SHADER_TTF_LABEL);
//    if(self->dirty & SPRITE_COLOR_DIRTY) {
//        float c4f[4] = {1.0f};
//        color_vec4(self->color, c4f);
//        render_set_unfiorm(R, "mix_color", UNIFORM_4F, c4f);
//    }
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
    
    self->dirty |= SPRITE_FRAME_DIRTY;
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

    self->dirty |= SPRITE_TRANSFORM_DIRTY;
}

void sprite_set_rotation(struct sprite* self, float rotation) {
    self->rotation = rotation;
    
    self->dirty |= SPRITE_ROTATION_DIRTY;
}
void sprite_set_scale(struct sprite* self, float scale) {
    self->scale_x = self->scale_y = scale;
    
    self->dirty |= SPRITE_SCALE_DIRTY;
}

void sprite_set_color(struct sprite* self, color color) {
    self->color = color;
    
    self->dirty |= SPRITE_COLOR_DIRTY;
}
