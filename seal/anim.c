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


static unsigned int __anim_id = 0;
struct anim* anim_new(struct array* sprite_frames)
{
    struct anim* anim = STRUCT_NEW(anim);

    anim->interval = 1.0f/4.0f;
    anim->callback = NULL;
    anim->sprite_frames = array_copy(sprite_frames);
    anim->speed = 1.0f;

    // privates
    anim->__id = ++__anim_id;
    anim->__now = 0.0f;
    anim->__cur_frame = 0;
    anim->__total_frame = array_size(anim->sprite_frames);
    anim->__state = ANIM_STATE_PLAY;
    return anim;
}

void anim_free(struct anim* self)
{
    array_free(self->sprite_frames);
    s_free(self);
}

void anim_update(struct anim* self, float dt)
{
    if(self->__state != ANIM_STATE_PLAY) {
        return;
    }

    float next = self->__now + dt * self->speed;
    if(next >= self->interval) {
        self->__now = 0.0f;
        self->__cur_frame = (++self->__cur_frame) % self->__total_frame;
    } else {
        self->__now = next;
    }
}

void anim_play(struct anim* self)
{
    if(self->__state == ANIM_STATE_STOPPED) {
        self->__state = ANIM_STATE_PLAY;
    }
}

void anim_stop(struct anim* self)
{
    if(self->__state != ANIM_STATE_STOPPED) {
        self->__state = ANIM_STATE_STOPPED;
    }
}

void anim_resume(struct anim* self)
{
    if(self->__state == ANIM_STATE_PAUSED) {
        self->__state = ANIM_STATE_PLAY;
    }
}

void anim_set_interval(struct anim* self, float interval)
{
    self->interval = interval;
}

void anim_set_speed(struct anim* self, float speed)
{
    self->speed = speed;
}

void anim_set_callback(struct anim* self, void (*callback)(struct anim*) )
{
    self->callback = callback;
}

struct sprite_frame* anim_current_frame(struct anim* self)
{
    return anim_frame(self, self->__cur_frame);
}

struct sprite_frame* anim_frame(struct anim* self, unsigned int frame_index)
{
    return array_at(self->sprite_frames, frame_index);
}

