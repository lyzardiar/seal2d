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


#ifndef anim_h
#define anim_h

#include "seal_base.h"

// WC - todo fix
#include "base/array.h"

struct sprite_frame;

#define MAX_FRAME (60)

#define ANIM_STATE_PLAY 0
#define ANIM_STATE_PAUSED 1
#define ANIM_STATE_STOPPED 2

struct anim {
    unsigned int __id;
    unsigned int __cur_frame;
    unsigned int __total_frame;
    int __state;
    float __now;

    struct array* sprite_frames; // only have a reference of the frames
    float interval;
    float speed;
    void (*callback)(struct anim*);
};

struct anim* anim_new(struct array* sprite_frames);
void anim_free(struct anim* self);

void anim_update(struct anim* self, float dt);
void anim_play(struct anim* self);
void anim_stop(struct anim* self);
void anim_resume(struct anim* self);
void anim_set_interval(struct anim* self, float interval);
void anim_set_speed(struct anim* self, float speed);
void anim_set_callback(struct anim* self, void (*callback)(struct anim*) );


struct sprite_frame* anim_current_frame(struct anim* self);
struct sprite_frame* anim_frame(struct anim* self, unsigned int frame_index);

#endif