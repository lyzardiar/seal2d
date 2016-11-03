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


#ifndef render_h
#define render_h

#include "seal_base.h"

// Header dependenceis here

// #include "math/geo.h"
// #include "shader.h"
// #include "sprite.h"

#define MAX_OBJECTS (1024) // max objects in on drawcall

// we may use 1 draw call at most for each of the object
// the implemention could be improved for sure,
// TODO: consider a better implemention.
#define MAX_RENDER_BATCH (MAX_OBJECTS)

#define INVALID_ATTR_LOCATION (-1)

#define RENDER_MASK_SCISSORS (1 << 0)

struct render;

enum RENDER_TYPE {
    RENDER_TYPE_SPRITE = 0,
    RENDER_TYPE_PRIMITIVE,
    RENDER_TYPE_SPINE,

    RENDER_MAX,
};

struct render_func {
    void (*init)(struct render*);
    void (*start)(struct render*);
    void (*end)(struct render*);
    void (*flush)(struct render*);
    void (*destroy)(struct render*);
};

struct render_object {
    enum RENDER_TYPE type;

    struct render_func render_func;
    void* context;
};

struct vertex_buffer {
    // vertex data
    GLuint vao;
    GLuint vbo;
    struct vertex* data;
    int offset;

    // index buffer
    GLuint vibo;
    GLushort* idata;
};

struct attr_location {
    GLuint position;
    GLuint color;
    GLuint uv;
};

struct render_state {
    GLuint program;
    struct attr_location loc;
};

struct render_batch {
    int n_objects;
    int offset;
    GLint tex_id;
};

/*
 *  render context class
 */

struct render_context {
    struct render_batch batches[MAX_RENDER_BATCH];
    struct render_batch* current_batch;
    int current_batch_index;

    struct render_state state;
    int n_objects;
};

#define render_context_nbatches(ctx) ((ctx)->current_batch_index+1)
void render_context_reset(struct render_context* self);
bool render_context_object_incfull(struct render_context* self);
struct render_batch* render_context_pop_batch(struct render_context* self);
bool render_context_update_batch(struct render_context* self,
                                 int offset,
                                 int tex_id);

/*
 * render class
 * it's instance is stored in the global GAME context
 */
struct render {
    struct render_object R_objs[RENDER_MAX]; // each render has a target object

    struct shader* shader;  // shader instance.
    int last;               // last render type.
    int current;            // current render type.

    int drawcall;

    unsigned int masks;
    struct rect scissors;
};

struct render* render_new();
void render_free(struct render* self);

void* render_get_context(struct render* self, enum RENDER_TYPE type);
void render_set_object(struct render* self, struct render_object* object);
void render_set_mvp(GLuint program, float* mat);
void render_scissors_test(struct render* self);
void render_set_scissors(struct render* self, int x, int y, int width, int height);
void render_clean_scissors(struct render* self);

void render_clear(struct render* self, color c);
void render_switch(struct render* self, enum RENDER_TYPE type);
void render_flush(struct render* self);


/*----------------------------------------------------------------------------*/

#endif
