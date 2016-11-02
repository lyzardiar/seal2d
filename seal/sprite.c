#include <string.h>
#include <math.h>

#include "platform/fs.h"

#include "base/hashmap.h"
#include "seal.h"
#include "memory.h"

#include "texture.h"
#include "ttf_font.h"
#include "sprite.h"
#include "anim.h"
#include "bmfont.h"
#include "spine_anim.h"
#include "render.h"
#include "event.h"
#include "scheduler.h"

#include "lua_handler.h"

#include "util.h"

#include "renders/primitive_render.h"
#include "renders/sprite_render.h"
#include "renders/spine_render.h"


EXTERN_GAME;

// dirty flags
#define SPRITE_TRANSFORM_DIRTY  (1 << 0)
#define SPRITE_SCALE_DIRTY      (1 << 1)
#define SPRITE_ROTATION_DIRTY   (1 << 2)
#define SPRITE_COLOR_DIRTY      (1 << 3)
#define SPRITE_FRAME_DIRTY      (1 << 4)
#define SPRITE_ZORDER_DIRTY     (1 << 5)

#define SPRITE_SRT_DIRTY        (SPRITE_TRANSFORM_DIRTY | SPRITE_SCALE_DIRTY | SPRITE_ROTATION_DIRTY)

#define SPRITE_ALL_DIRTY        (SPRITE_SRT_DIRTY | SPRITE_COLOR_DIRTY | SPRITE_FRAME_DIRTY)

static unsigned int __sprite_id = 0;
static struct render* R = NULL;
static struct sprite_frame_cache* C = NULL;
void sprite_init_render(struct render* render)
{
    R = render;
}

static int hash_str(void* key)
{
    return hashmapHash(key, strlen(key));
}

static bool hash_equal(void* a, void* b)
{
    return strcmp(a, b) == 0;
}


static void transform_vertex(struct primitive_vertex* v,
                             struct affine* transform)
{
    v->position[0] += transform->x;
    v->position[1] += transform->y;
}

static void sprite_update_primitive_line_transform(struct sprite* self,
                                                   struct affine* transform)
{
    for (int i = 0; i < 2; ++i) {
        transform_vertex(&self->primitive_data.primitive_vertex[i],
                         transform);
    }
}

static void sprite_update_primitive_rect_transform(struct sprite* self,
                                                   struct affine* transform)
{
    for (int i = 0; i < 4; ++i) {
        transform_vertex(&self->primitive_data.primitive_vertex[i],
                         transform);
    }
}

static void sprite_sort_zorder(struct sprite* self)
{
    if (self->dirty & SPRITE_ZORDER_DIRTY) {
        int n = array_size(self->children);
        for (int i = 0; i < n-1; ++i) {
            for (int j = i+1; j < n; ++j) {
                struct sprite* a = array_at(self->children, i);
                struct sprite* b = array_at(self->children, j);
                if (a && b && a->zorder > b->zorder) {
                    array_swap(self->children, i, j);
                }
            }
        }

        self->dirty &= (~SPRITE_ZORDER_DIRTY);
    }
}

static void sprite_update_anim(struct sprite* self, float dt)
{
    if (self->type == SPRITE_TYPE_PIC) {
        struct anim* anim = self->sprite_data.anim;
        if (anim) {
            anim_update(anim, dt);
            sprite_set_sprite_frame(self, anim_current_frame(anim));
        }
    }
}

static void sprite_update_scale9(struct sprite* self)
{
    if (self->type == SPRITE_TYPE_SCALE9) {
        struct sprite_frame* frame = self->scale9_data.frame;
        struct rect* inset = &(self->scale9_data.inset);
        int width = frame->source_size.width;
        int height = frame->source_size.height;
        
        float ox = -self->width * self->anchor_x;
        float oy = self->height * self->anchor_y;
        
        struct scale9_data* data = &(self->scale9_data);
        
        int l = inset->x;
        int c = inset->width;
        int r = width - (inset->x + inset->width);
        int p = self->width - r;
        float ps = (self->width - (l + r)) * 1.0f / c;
        
        int t = inset->y;
        int m = inset->height;
        int b = height - (inset->y + inset->height);
        int q = self->height - b;
        float qs = (self->height - (t + b)) * 1.0f / m;
        
        sprite_set_scale_x(data->tc, ps);
        sprite_set_scale_x(data->mc, ps);
        sprite_set_scale_x(data->bc, ps);
        
        sprite_set_scale_y(data->ml, qs);
        sprite_set_scale_y(data->mc, qs);
        sprite_set_scale_y(data->mr, qs);
        
        sprite_set_pos(data->tl, ox+0, oy+0);
        sprite_set_pos(data->tc, ox+l, oy+0);
        sprite_set_pos(data->tr, ox+p, oy+0);
        sprite_set_pos(data->ml, ox+0, oy-t);
        sprite_set_pos(data->mc, ox+l, oy-t);
        sprite_set_pos(data->mr, ox+p, oy-t);
        sprite_set_pos(data->bl, ox+0, oy-q);
        sprite_set_pos(data->bc, ox+l, oy-q);
        sprite_set_pos(data->br, ox+p, oy-q);
    }
}

static void sprite_update_transform(struct sprite* self)
{
    // pass the dirty flags to the children
    for (int i = 0; i < array_size(self->children); ++i) {
        struct sprite* child = (struct sprite*)array_at(self->children, i);
        if (child) {
            // we should only pass the SRT dirty to the children
            child->dirty |= (self->dirty & SPRITE_SRT_DIRTY);
        }
    }

    // TODO: we could improve performance by seprating transform from SRT,
    // but for simpler implemention, we have use SRT_DIRTY right now. :)
    if (self->dirty & SPRITE_SRT_DIRTY) {
        struct affine* local = &self->local_srt;
        af_srt(local, self->x, self->y, self->scale_x, self->scale_y, self->rotation);

        struct affine tmp;
        af_identify(&tmp);
        af_concat(&tmp, &self->local_srt);

        if (self->parent) {
            af_concat(&tmp, &(self->parent->world_srt));
        }

        // TODO: refactor here someday. doesn't have any good idea right now.
        if (self->type == SPRITE_TYPE_PRIMITVE) {
            switch (self->primitive_data.primitive_type) {
                case PRIMITIVE_LINE:
                    sprite_update_primitive_line_transform(self, &tmp);
                    break;
                case PRIMITIVE_RECT:
                    sprite_update_primitive_rect_transform(self, &tmp);
                    break;
                default:
                    break;
            }
        }
        else if (self->type == SPRITE_TYPE_SCALE9) {
            sprite_update_scale9(self);
        }
        else {
            float w0 = self->width * (1-self->anchor_x);
            float w1 = self->width * (0-self->anchor_x);

            float h0 = self->height * (1-self->anchor_y);
            float h1 = self->height * (0-self->anchor_y);
            float a = tmp.a;
            float b = tmp.b;
            float c = tmp.c;
            float d = tmp.d;
            float tx = tmp.x;
            float ty = tmp.y;

            float x0 = a * w1 + c * h1 + tx;
            float y0 = d * h1 + b * w1 + ty;
            float x1 = a * w0 + c * h1 + tx;
            float y1 = d * h1 + b * w0 + ty;
            float x2 = a * w0 + c * h0 + tx;
            float y2 = d * h0 + b * w0 + ty;
            float x3 = a * w1 + c * h0 + tx;
            float y3 = d * h0 + b * w1 + ty;

            struct glyph* g = &self->sprite_data.glyph;
            SET_VERTEX_POS(g->bl, x0, y0);
            SET_VERTEX_POS(g->br, x1, y1);
            SET_VERTEX_POS(g->tr, x2, y2);
            SET_VERTEX_POS(g->tl, x3, y3);

            self->width = fabs(x1 - x0);
            self->height = fabs(y3 - y1);
        }
        self->world_srt = tmp;
    }
    self->dirty &= (~SPRITE_SRT_DIRTY);
}

static void sprite_update_color(struct sprite* self)
{
    if (self->dirty & SPRITE_COLOR_DIRTY) {
        
        // todo: other type wants his bev
        switch(self->type) {
            case SPRITE_TYPE_SCALE9:
            {
                struct scale9_data* data = &(self->scale9_data);
                for (int i = 0; i < 9; ++i){
                    sprite_set_color(data->sprites[i], self->color);
                }
                break;
            }
            default:
            {
                struct glyph* g = &self->sprite_data.glyph;
                SET_VERTEX_COLOR_UINT(g->bl, self->color);
                SET_VERTEX_COLOR_UINT(g->br, self->color);
                SET_VERTEX_COLOR_UINT(g->tr, self->color);
                SET_VERTEX_COLOR_UINT(g->tl, self->color);
                break;
            }
        }
        self->dirty &= (~SPRITE_COLOR_DIRTY);
    }
}

struct sprite_frame_cache* sprite_frame_cache_new()
{
    struct sprite_frame_cache* c = STRUCT_NEW(sprite_frame_cache);
    c->cache = hashmapCreate(128, hash_str, hash_equal);
    
    C = c;
    return c;
}

void sprite_frame_cache_free(struct sprite_frame_cache* self)
{
    hashmapFree(self->cache);
    s_free(self);
}

void sprite_frame_cache_add(struct sprite_frame_cache* self,
                            struct sprite_frame* frame)
{
    hashmapPut(self->cache, frame->key, frame);
}

struct sprite_frame* sprite_frame_cache_get(struct sprite_frame_cache* self,
                                            const char* key)
{
    struct sprite_frame* f = hashmapGet(self->cache, (void*)key);
    if (!f) {
        f = sprite_frame_new(key);
        sprite_frame_cache_add(GAME->sprite_frame_cache, f);
    }
    return f;
}

struct sprite_frame* sprite_frame_new(const char* key)
{
    struct sprite_frame* f = STRUCT_NEW(sprite_frame);
    memset(f, 0, sizeof(struct sprite_frame));
    
    size_t len = strlen(key);
    f->key = s_malloc(len+1);
    memset(f->key, 0, len+1);
    strcpy(f->key, key);
    
    return f;
}

void sprite_frame_free(struct sprite_frame* self)
{
    s_free(self->key);
    s_free(self);
}

void sprite_frame_set_texture_id(struct sprite_frame* self, GLuint tex_id)
{
    self->tex_id = tex_id;
}

void sprite_frame_init_uv(struct sprite_frame* self,
                          float texture_width, float texture_height) {
    struct rect* frame_rect = &self->frame_rect;
    self->uv.u = frame_rect->x / texture_width;
    self->uv.v = 1.0f - (frame_rect->y + frame_rect->height) / texture_height; // left corner is (0, 0)
    self->uv.w = frame_rect->width / texture_width;
    self->uv.h = frame_rect->height / texture_height;
}

void sprite_frame_init_subuv(struct sprite_frame* self, struct sprite_frame* parent)
{
    int tex_width = parent->frame_rect.width / parent->uv.w;
    int tex_height = parent->frame_rect.height / parent->uv.h;
    sprite_frame_init_uv(self, tex_width, tex_height);
}

void sprite_frame_tostring(struct sprite_frame* self, char* buff)
{
    sprintf(buff, "{key = \"%s\",\n"
            "frame_rect = {%d, %d, %d, %d},\n"
            "source_rect = {%d, %d, %d, %d},\n"
            "size = {%d, %d},\n"
            "text_id = %d,\n"
            "rotated = %s,\n"
            "trimmed = %s,\n"
            "uv = {%.2f, %.2f, %.2f, %.2f}}\n",
            self->key,
            self->frame_rect.x, self->frame_rect.y,
            self->frame_rect.width, self->frame_rect.height,
            self->source_rect.x, self->source_rect.y,
            self->source_rect.width, self->source_rect.height,
            self->source_size.width, self->source_size.height,
            self->tex_id,
            stringfy_bool(self->rotated),
            stringfy_bool(self->trimmed),
            self->uv.u, self->uv.v, self->uv.w, self->uv.h);
}

static void sprite_init(struct sprite* self,
                        enum sprite_type type,
                        float width, float height)
{
    self->__id = ++__sprite_id;
    self->parent = NULL;
    self->dirty = SPRITE_ALL_DIRTY;
    self->zorder = 0;
    self->scale_x = self->scale_y = 1.0f;
    self->rotation = 0.0f;
    self->x = self->y = 0.0f;
    self->anchor_x = self->anchor_y = 0.5f;
    self->width = width;
    self->height = height;
    self->swallow = true;
    self->color = C4B_COLOR(255, 255, 255, 255);
    self->visible = true;
    self->children = array_new(16);

    af_identify(&self->local_srt);
    af_identify(&self->world_srt);
}

static void sprite_set_glyph(struct sprite* self, struct rect* rect,
                      struct uv* uv, GLuint tex_id)
{
    struct glyph* g = &(self->sprite_data.glyph);
    
    SET_VERTEX_POS(g->bl, 0.0f, 0.0f);
    SET_VERTEX_COLOR(g->bl, 255, 255, 255, 255);

    SET_VERTEX_POS(g->br, rect->width, 0.0f);
    SET_VERTEX_COLOR(g->br, 255, 255, 255, 255);

    SET_VERTEX_POS(g->tl, 0.0f, rect->height);
    SET_VERTEX_COLOR(g->tl, 255, 255, 255, 255);

    SET_VERTEX_POS(g->tr, rect->width, rect->height);
    SET_VERTEX_COLOR(g->tr, 255, 255, 255, 255);

    if (uv) {
        SET_VERTEX_UV(g->bl, uv->u,         uv->v);
        SET_VERTEX_UV(g->br, uv->u + uv->w, uv->v);
        SET_VERTEX_UV(g->tl, uv->u,         uv->v + uv->h);
        SET_VERTEX_UV(g->tr, uv->u + uv->w, uv->v + uv->h);
    }
}

struct sprite* sprite_new(struct sprite_frame* frame)
{
    struct sprite* s = STRUCT_NEW(sprite);
    s->type = SPRITE_TYPE_PIC;
    s->sprite_data.anim = NULL;
    s->sprite_data.frame = frame;
    sprite_init(s, SPRITE_TYPE_PIC, frame->source_rect.width, frame->source_rect.height);

    sprite_set_glyph(s, &frame->frame_rect, &frame->uv, frame->tex_id);

    // TODO: should we do it here? can this make the glyph right?
    // update the sprite once, so that we can get glyph right after we new it
    // sprite_update_transform(s);
    return s;
}

struct sprite* sprite_new_label(const char* label)
{
    struct sprite* s = STRUCT_NEW(sprite);
    s->type = SPRITE_TYPE_TTF_LABEL;

    //TODO: implement this later
    s_assert(false);
    return s;
}

struct sprite* sprite_new_bmfont_label(const char* label,
                                       const char* fnt_path,
                                       int line_width)
{
    struct rect r = {0, 0, line_width, 0};
    struct sprite* s = sprite_new_container(&r);
    s->type = SPRITE_TYPE_BMFONT_LABEL;
    
    struct bmfont* bmfont = bmfont_cache_get(GAME->bmfont_cache, fnt_path);

    s->bmfont_data.line_width = line_width;
    s->bmfont_data.text = NULL;
    s->bmfont_data.bmfont = bmfont;
    sprite_set_text(s, label);

    return s;
}

struct sprite* sprite_new_container(struct rect* r)
{
    struct sprite* s = STRUCT_NEW(sprite);
    s->type = SPRITE_TYPE_CONTAINER;
    
    sprite_init(s, SPRITE_TYPE_CONTAINER, r->width, r->height);
    return s;
}

struct sprite* sprite_new_spine(const char* atlas_path,
                                const char* spine_data_path,
                                float scale)
{
    struct sprite* s = STRUCT_NEW(sprite);
    s->type = SPRITE_TYPE_SPINE;

    struct rect r;
    struct spine_anim* spine_anim = spine_anim_new(atlas_path,
                                                   spine_data_path,
                                                   scale);
    spine_get_boundingbox(spine_anim, &r);
    sprite_init(s, SPRITE_TYPE_SPINE, r.width, r.height);
    s->spine_data.spine_anim = spine_anim;
    return s;
}

struct sprite* sprite_new_clip(struct rect* r)
{
    struct sprite* s = STRUCT_NEW(sprite);
    s->type = SPRITE_TYPE_CLIP;
    
//    sprite_init(s, r->width, r->height);
//    sprite_set_glyph(s, r, NULL, 0);
//    
//    s->x = r->x;
//    s->y = r->y;
    return s;
}

// vertex: float[4], start point:(v[0], v[1]), end point: (v[1], v[2])
struct sprite* sprite_new_line(float* vertex, float width, color line_color)
{
    struct sprite* s = STRUCT_NEW(sprite);

    struct rect rect = {
        0, 0, vertex[2] - vertex[0], width
    };

    s->type = SPRITE_TYPE_PRIMITVE;
    sprite_init(s, SPRITE_TYPE_PRIMITVE, rect.width, rect.height);
    sprite_set_color(s, line_color);

    s->primitive_data.primitive_type = PRIMITIVE_LINE;
    struct primitive_vertex* v = s_malloc(PRIMITIVE_VERTEX_SIZE * 2);
    SET_VERTEX_POS(v[0], vertex[0], vertex[1]);
    SET_VERTEX_POS(v[1], vertex[2], vertex[3]);

    SET_VERTEX_COLOR_UINT(v[0], line_color);
    SET_VERTEX_COLOR_UINT(v[1], line_color);

    s->primitive_data.primitive_vertex = v;
    return s;
}

struct sprite* sprite_new_rect(struct rect* rect,
                               unsigned int rect_flag,
                               color fill_color, color outline_color)
{
    struct sprite* s = STRUCT_NEW(sprite);
    s->type = SPRITE_TYPE_PRIMITVE;
    struct primitive_data* primitive_data = &s->primitive_data;
    primitive_data->primitive_type = PRIMITIVE_RECT;

    sprite_init(s, SPRITE_TYPE_PRIMITVE, rect->width, rect->height);
    sprite_set_glyph(s, rect, NULL, 0);
    sprite_set_color(s, fill_color);

    struct primitive_vertex* v = s_malloc(PRIMITIVE_VERTEX_SIZE * 4);
    float l = rect->x;
    float r = rect->x + rect->width;
    float b = rect->y;
    float t = rect->y + rect->height;

    // 4 lines forms an rect.
    SET_VERTEX_POS(v[0], l, b);
    SET_VERTEX_POS(v[1], r, b);
    SET_VERTEX_POS(v[2], r, t);
    SET_VERTEX_POS(v[3], l, t);

    SET_VERTEX_COLOR_UINT(v[0], fill_color);
    SET_VERTEX_COLOR_UINT(v[1], fill_color);
    SET_VERTEX_COLOR_UINT(v[2], fill_color);
    SET_VERTEX_COLOR_UINT(v[3], fill_color);

    primitive_data->primitive_vertex = v;
    primitive_data->rect_flag = rect_flag;
    primitive_data->fill_color = fill_color;
    primitive_data->outline_color = outline_color;

    return s;
}

static struct sprite* sprite_new_scale9_item(struct sprite* self,
                                             int x, int y,
                                             int width, int height,
                                             int tex_width, int tex_height,
                                             GLuint tex_id,
                                             const char* tag)
{
    struct sprite_frame* frame = sprite_frame_new("");
    frame->frame_rect.x = x;
    frame->frame_rect.y = y;
    frame->frame_rect.width = width;
    frame->frame_rect.height = height;
    
    frame->source_rect = frame->frame_rect;
    
    frame->source_size.width = width;
    frame->source_size.height = height;
    
    frame->tex_id = tex_id;
    
    sprite_frame_init_uv(frame, tex_width, tex_height);
    frame->__initialized = true;
    
    struct sprite* s = sprite_new(frame);
    sprite_set_anchor(s, 0, 1);
    sprite_add_child(self, s, 0);
    return s;
}

struct sprite* sprite_new_scale9(struct sprite_frame* frame, struct rect* inset)
{
    int t_width = frame->frame_rect.width / frame->uv.w;
    int t_height = frame->frame_rect.height / frame->uv.h;
    
    int width = frame->source_size.width;
    int height = frame->source_size.height;
    
    struct rect rect = {0, 0, width, height};
    struct sprite* s = sprite_new_container(&rect);
    sprite_set_anchor(s, 0.5f, 0.5f);
    s->type = SPRITE_TYPE_SCALE9;
    
    int x = frame->frame_rect.x;
    int y = frame->frame_rect.y;
    
    int l = inset->x;
    int c = inset->width;
    int r = width - (inset->x + inset->width);
    int p = l + c;
    
    int t = inset->y;
    int m = inset->height;
    int b = height - (inset->y + inset->height);
    int q = t + m;
    
    GLuint tex_id = frame->tex_id;
    struct scale9_data* data = &(s->scale9_data);
    data->frame = frame;
    data->inset = *inset;
    
    data->tl = sprite_new_scale9_item(s, x+0, y+0, l, t, t_width, t_height, tex_id, "tlf");
    data->tc = sprite_new_scale9_item(s, x+l, y+0, c, t, t_width, t_height, tex_id, "tcf");
    data->tr = sprite_new_scale9_item(s, x+p, y+0, r, t, t_width, t_height, tex_id, "trf");
    
    data->ml = sprite_new_scale9_item(s, x+0, y+t, l, m, t_width, t_height, tex_id, "mlf");
    data->mc = sprite_new_scale9_item(s, x+l, y+t, c, m, t_width, t_height, tex_id, "mcf");
    data->mr = sprite_new_scale9_item(s, x+p, y+t, r, m, t_width, t_height, tex_id, "mrf");
    
    data->bl = sprite_new_scale9_item(s, x+0, y+q, l, b, t_width, t_height, tex_id, "blf");
    data->bc = sprite_new_scale9_item(s, x+l, y+q, c, b, t_width, t_height, tex_id, "bcf");
    data->br = sprite_new_scale9_item(s, x+p, y+q, r, b, t_width, t_height, tex_id, "brf");
    
    return s;
}

void sprite_free(struct sprite* self)
{
    switch (self->type) {
        case SPRITE_TYPE_PIC:
        {
            if (self->sprite_data.anim) {
                anim_free(self->sprite_data.anim);
            }
            break;
        }

        case SPRITE_TYPE_BMFONT_LABEL:
        {
            if (self->bmfont_data.text) {
                s_free(self->bmfont_data.text);
            }
        }

        case SPRITE_TYPE_SPINE:
        {
            if (self->spine_data.spine_anim) {
                spine_anim_free(self->spine_data.spine_anim);
            }
            break;
        }
            
        case SPRITE_TYPE_PRIMITVE:
        {
            if (self->primitive_data.primitive_vertex) {
                s_free(self->primitive_data.primitive_vertex);
            }
            break;
        }
            
        case SPRITE_TYPE_SCALE9:
        {
            struct scale9_data* data = &(self->scale9_data);
            for (int i = 0; i < 9; ++i){
                s_free(data->sprites[i]->sprite_data.frame);
                data->sprites[i]->sprite_data.frame = NULL;
            }
            break;
        }

        default:
            break;
    }
    s_free(self);
}

void sprite_set_text(struct sprite* self, const char* label)
{
    struct bmfont_data* __expaned_data = &self->bmfont_data;
    char* text = __expaned_data->text;
    struct bmfont* bmfont = __expaned_data->bmfont;
    if (text && (!strcmp(text, label))) {
        return;
    }
        
    if (self->type == SPRITE_TYPE_BMFONT_LABEL && bmfont) {
        sprite_remove_all_child(self);
        const char* fnt_path = bmfont->fnt_file;
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
        
        int width = __expaned_data->line_width;
        int height = bmfont->common.lineHeight;
        bool auto_calc_width = width == 0;

        // TODO: we should have an bmfont render, cause it wastes a lot of sprite visit
        // to render a simple label, anyway it's simpler right now. :)
        for (int i = 0; i < len; ++i) {
            
            // TODO: currently we only support ** ENGLISH **, we need have a UTF-8 character split function
            // to split this label into characters, so that we were able to support Chinese, Japanese..O
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
            int yoffset = bmfont->common.lineHeight - character->yoffset - character->height;
            sprite_set_pos(c_sprite, x + character->xoffset, y + yoffset);
            sprite_set_anchor(c_sprite, 0, 0);
            sprite_add_child(self, c_sprite, 0);
            
            // coord caculation
            x += character->xadvance;
            if (!auto_calc_width) {
                if ( x > width) {
                    x = 0;
                    y -= bmfont->common.lineHeight;
                    height += bmfont->common.lineHeight;
                }
            } else {
                width = x;
            }
        }

        sprite_set_size(self, width, height);
        
        int label_count = strlen(label);
        if (!text) {
            text = s_malloc(label_count + 1);

        } else if (label_count > strlen(text) ) {
            text = s_realloc(text, label_count);
        }
        strcpy(text, label);
        self->bmfont_data.text = text;
    }
}

void sprite_add_child(struct sprite* self, struct sprite* child, int zorder)
{
    s_assert(child && child != self);
    
    // TODO: when we add the child, search the first NULL position.
    // TODO: consider the ZORDER
    array_push_back(self->children, child);
    child->zorder = zorder;
    child->parent = self;

    self->dirty |= SPRITE_ZORDER_DIRTY;
}

void sprite_remove_from_parent(struct sprite* self)
{
    s_assert(self->parent);
    sprite_remove_child(self->parent, self);
}

void sprite_remove_child(struct sprite* self, struct sprite* child)
{
    // here we should release the memory??? yes.
    if (child) {
        scheduler_stop_target(GAME->scheduler, child);
        // we only remove the child, but we don't move the array
        array_remove(self->children, child);
        sprite_free(child);
    }
}

void sprite_remove_all_child(struct sprite* self)
{
    scheduler_stop_target(GAME->scheduler, self);
    struct array* children = self->children;
    for (int i = 0 ;i < array_size(children); ++i) {
        sprite_remove_child(self, array_at(children, i));
    }
}

static int touch_event_set_func(lua_State* L, void* ud)
{
    struct touch_event* event = (struct touch_event*)ud;
    lua_pushstring(L, SPRITE_EVENT_TYPE);
    lua_pushinteger(L, event->type);
    lua_pushinteger(L, event->x);
    lua_pushinteger(L, event->y);
    return 4;
}

void sprite_touch(struct sprite* self, struct touch_event* touch_event)
{
    if(touch_event->swallowd) {
        return;
    }
    
    struct array* children = self->children;
    for (int i = 0 ;i < array_size(children); ++i) {
        struct sprite* child = (struct sprite*)array_at(children, i);
        if (child) { // NULL indicates that the child has been removed
            // recursively visit the children.
            sprite_touch(child, touch_event);
            
            if(touch_event->swallowd) {
                return;
            }
        }
    }
    
    if (sprite_contains(self, touch_event->x, touch_event->y)) {
        if (self->swallow) {
            touch_event->swallowd = true;
        }
        
        seal_call_func(self, touch_event_set_func, touch_event, false);
    }
}

bool sprite_contains(struct sprite* self, float x, float y)
{
    struct glyph* g = &self->sprite_data.glyph;
    struct rect world = {
        g->bl.position[0],
        g->bl.position[1],
        self->width,
        self->height,
    };
    return rect_contains(&world, x, y);
}

void sprite_run_action(struct sprite* self, struct action* action)
{
    scheduler_schedule(GAME->scheduler, self, action);
}

static void sprite_draw_pic(struct sprite* self)
{
    render_switch(R, RENDER_TYPE_SPRITE);
    sprite_render_func_draw(R, self);
}

static void sprite_draw_spine(struct sprite* self, float dt)
{
    struct spine_anim* anim = self->spine_data.spine_anim;
    render_switch(R, RENDER_TYPE_SPINE);
    spine_anim_update(anim, dt);
    spine_anim_draw(anim, R, self->world_srt.x, self->world_srt.y);
}

static void sprite_draw_primitive(struct sprite* self)
{
    render_switch(R, RENDER_TYPE_PRIMITIVE);
    primitive_render_func_draw(R,
                               self->primitive_data.primitive_type,
                               self);
}

static void sprite_draw(struct sprite* self, float dt)
{
    if (!self->visible) {
        return;
    }

    switch (self->type) {
        case SPRITE_TYPE_PIC:
            sprite_draw_pic(self);
            break;
        case SPRITE_TYPE_SPINE:
            sprite_draw_spine(self, dt);
            break;
        case SPRITE_TYPE_PRIMITVE:
            sprite_draw_primitive(self);
            break;
        case SPRITE_TYPE_CONTAINER:
            // do nothing.
            break;
        default:
            break;
    }
}

static void sprite_after_visit(struct sprite* self)
{

}

void sprite_visit(struct sprite* self, float dt)
{
    sprite_sort_zorder(self);
    sprite_update_transform(self);
    sprite_update_anim(self, dt);
    sprite_update_color(self);
    sprite_draw(self, dt);

    struct array* children = self->children;
    for (int i = 0 ;i < array_size(children); ++i) {
        struct sprite* child = (struct sprite*)array_at(children, i);
        if (child) { // NULL indicates that the child has been removed
            // recursively visit the children.
            sprite_visit(child, dt);
        }
    }
    sprite_after_visit(self);
}

void sprite_draw_label(struct sprite* self)
{

}

void sprite_draw_clip(struct sprite* self)
{
//    struct rect r = {self->world_srt.x, self->world_srt.y, self->width, self->height};
//    render_set_scissors(R, &r);
}

void sprite_clean_clip(struct sprite* self)
{
//    render_clear_scissors(R);
}

void sprite_set_sprite_frame(struct sprite* self, struct sprite_frame* frame)
{
    struct glyph* g = sprite_get_glyph(self);
    struct uv* uv = &frame->uv;
    SET_VERTEX_UV(g->bl, uv->u,         uv->v);
    SET_VERTEX_UV(g->br, uv->u + uv->w, uv->v);
    SET_VERTEX_UV(g->tl, uv->u,         uv->v + uv->h);
    SET_VERTEX_UV(g->tr, uv->u + uv->w, uv->v + uv->h);

    self->sprite_data.frame = frame;
    
    self->dirty |= SPRITE_FRAME_DIRTY;
}

void sprite_set_anim(struct sprite* self, struct anim* anim)
{
    struct anim* origin = self->sprite_data.anim;
    if (origin != anim) {
        if(origin) {
            anim_free(origin);
        }
        
        self->sprite_data.anim = anim;
        anim_play(anim);
    }
}

void sprite_set_spine_anim(struct sprite* self, const char* anim_name, int track, bool loop)
{
    spine_anim_set_anim(self->spine_data.spine_anim, anim_name, track, loop);
}

void sprite_set_visible(struct sprite* self, bool visible)
{
    self->visible = visible;
}

void sprite_set_pos(struct sprite* self, float x, float y)
{
    self->x = x;
    self->y = y;

    self->dirty |= SPRITE_TRANSFORM_DIRTY;
}

void sprite_set_anchor(struct sprite* self, float x, float y)
{
    self->anchor_x = x;
    self->anchor_y = y;
    self->dirty |= SPRITE_TRANSFORM_DIRTY;
}

void sprite_set_rotation(struct sprite* self, float rotation)
{
    self->rotation = rotation;
    self->dirty |= SPRITE_ROTATION_DIRTY;
}

void sprite_set_scale(struct sprite* self, float scale)
{
    self->scale_x = self->scale_y = scale;
    self->dirty |= SPRITE_SCALE_DIRTY;
}

void sprite_set_scale_x(struct sprite* self, float scale_x)
{
    self->scale_x = scale_x;
    self->dirty |= SPRITE_SCALE_DIRTY;
}

void sprite_set_scale_y(struct sprite* self, float scale_y)
{
    self->scale_y = scale_y;
    self->dirty |= SPRITE_SCALE_DIRTY;
}

void sprite_set_color(struct sprite* self, color color)
{
    self->color = color;
    self->dirty |= SPRITE_COLOR_DIRTY;
}

void sprite_set_opacity(struct sprite* self, unsigned char opacity)
{
    self->color = (self->color & 0xffffff00) | opacity;
    self->dirty |= SPRITE_COLOR_DIRTY;
}

void sprite_set_size(struct sprite* self, float width, float height)
{
    self->width = width;
    self->height = height;
    self->dirty |= SPRITE_SRT_DIRTY;
}

struct glyph* sprite_get_glyph(struct sprite* self)
{
    return &self->sprite_data.glyph;
}

void sprite_dump_children(struct sprite* self)
{
    struct array* children = self->children;
    for (int i = 0; i < array_size(children); ++i) {
        struct sprite* s = array_at(children, i);
        if (s) {
            printf("[sprite] : addr(%p)"
                   "__id(%d) \n"
                   "children(%lu) \n",
                    s,
                    s->__id,
                    s->children != NULL ? array_size(s->children) : 0);
        } else {
            printf("[sprite] : NULL\n");
        }
    }
}
