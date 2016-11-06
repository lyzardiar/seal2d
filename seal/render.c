/*
 * Copyright (C) 2016 Tang Yiyang
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See BELOW for details.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "seal.h"

EXTERN_GAME;
#define RENDER_INVALID (-1)

extern void sprite_render_func_init(struct render* R);
extern void primitive_render_func_init(struct render* R);
extern void spine_render_func_init(struct render* R);

void render_context_reset(struct render_context* self)
{
    memset(self, 0, sizeof(struct render_context));

    self->current_batch = NULL;
    self->current_batch_index = 0;
    self->n_objects = 0;
}

bool render_context_object_incfull(struct render_context* self)
{
    return self->n_objects+1 > MAX_OBJECTS;
}

struct render_batch* render_context_pop_batch(struct render_context* self)
{
    return &self->batches[self->current_batch_index+1];
}

bool render_context_update_batch(struct render_context* self,
                                 int offset,
                                 int tex_id)
{
    struct render_batch* nb = render_context_pop_batch(self);
    nb->n_objects = 1;
    nb->offset = offset;
    nb->tex_id = tex_id;
    self->current_batch = nb;
    int index = ++self->current_batch_index;
    self->current_batch_index %= MAX_RENDER_BATCH;
    return index >= MAX_RENDER_BATCH;
}

struct render* render_new()
{
    struct render* r = STRUCT_NEW(render);
    r->last = r->current = RENDER_INVALID;
    r->masks = 0;

    sprite_render_func_init(r);
    primitive_render_func_init(r);
    spine_render_func_init(r);

    r->shader = shader_new();
    return r;
}

void render_free(struct render* self)
{
    shader_free(self->shader);
    s_free(self);
}

void* render_get_context(struct render* self, enum RENDER_TYPE type)
{
    return self->R_objs[type].context;
}

void render_set_object(struct render* self, struct render_object* object)
{
    memcpy(&self->R_objs[object->type], object, sizeof(*object));
}

void render_set_mvp(GLuint program, float* mat)
{
    GLint projection = glGetUniformLocation(program, "mvp");
    glUniformMatrix4fv(projection, 1, GL_FALSE, mat);
}

void render_set_scissors(struct render* self, int x, int y, int width, int height)
{
    s_assert(width > 0 && height > 0);

    self->scissors.x = x;
    self->scissors.y = y;
    self->scissors.width = width;
    self->scissors.height = height;

    self->masks |= RENDER_MASK_SCISSORS;
}

void render_scissors_test(struct render* self)
{
    if (self->masks & RENDER_MASK_SCISSORS) {
        glEnable(GL_SCISSOR_TEST);
        glScissor(self->scissors.x*GAME->config.scale_factor,
                  self->scissors.y*GAME->config.scale_factor,
                  self->scissors.width*GAME->config.scale_factor,
                  self->scissors.height*GAME->config.scale_factor);
    }
}

void render_clean_scissors(struct render* self)
{
    glDisable(GL_SCISSOR_TEST);
    self->masks &= (~RENDER_MASK_SCISSORS);
}

void render_clear(struct render* self, color c)
{
    float r = ((c >> 24) & 0xff) / 255.0;
    float g = ((c >> 16) & 0xff) / 255.0;
    float b = ((c >> 8 ) & 0xff) / 255.0;
    float a = ((c      ) & 0xff) / 255.0;

    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    self->drawcall = 0;
}

void render_switch(struct render* self, enum RENDER_TYPE type)
{
    if (self->current != type) {
        if (self->last != RENDER_INVALID) {
            struct render_object* LR = &self->R_objs[self->last];
            LR->render_func.flush(self);
        }
    }

    struct render_object* render_object = &self->R_objs[type];
    render_object->render_func.start(self);

    self->current = type;
    self->last = self->current;
}

void render_flush(struct render* self)
{
    if(self->current != RENDER_INVALID) {
        struct render_object* LR = &self->R_objs[self->current];
        LR->render_func.flush(self);
    }
}
