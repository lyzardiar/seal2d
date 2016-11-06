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


#ifndef __seal__action__
#define __seal__action__

#include "seal_base.h"

// Header dependencies
#include "lua_handler.h"

struct sprite;
struct Hashmap;
struct array;

enum action_type {
    ACTION_MOVE_TO = 0,
    ACTION_SCALE_TO,
    ACTION_FADE_TO,
    ACTION_EASE_IN,
    ACTION_SEQUENCE,
    ACTION_CALL,
    ACTION_TYPE_MAX,
};

enum action_state {
    ACTION_STATE_READY = 0,
    ACTION_STATE_RUNNING,
    ACTION_STATE_PAUSED,
    ACTION_STATE_STOPPED,
};

#define ACTION_SEQUENCE_MAX (8)
struct action_interval {
    float current;
    float duration;
};

struct action_move {
    struct action_interval __super;
    float from_x, from_y;
    float to_x, to_y;
};

struct action_scale {
    struct action_interval __super;
    float from_x, from_y;
    float to_x, to_y;
};

struct action_fade_to {
    struct action_interval __super;
    unsigned char from, to;
};

struct action_ease_in {
    struct action_interval __super;
    float rate;
    struct action* wrapped;
};

struct action_sequence {
    void* sequence[ACTION_SEQUENCE_MAX];
    int running_index;
    int n;
};

struct action_call_lua_func {
    LUA_FUNCTION_HANDLER lua_func;
};

struct action {
    unsigned long __id;
    enum action_type type;
    enum action_state state;
    union {
        struct action_move action_move;
        struct action_scale action_sacle;
        struct action_fade_to action_fade_to;
        struct action_ease_in action_ease_in;
        struct action_sequence action_sequence;
        struct action_call_lua_func action_call;
    };
};


void action_interval_init(struct action_interval* self, float duration);
bool action_interval_update(struct action_interval* self, float dt);

struct action* move_to(float duration, float to_x, float to_y);
struct action* scale_to(float duration, float to_x, float to_y);
struct action* fade_to(float duration, unsigned char to);
struct action* ease_in(struct action* action, float rate);
struct action* sequence(struct action* actions[], int n);
struct action* call_lua_func();

void action_play(struct action* self, struct sprite* target);
void action_stop(struct action* self);
void action_pause(struct action* self);
void action_resume(struct action* self);

bool action_is_stop(struct action* self);

bool action_update(struct action* self, struct sprite* sprite, float dt);
void action_free(struct action* self);

#endif
