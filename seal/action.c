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

void action_interval_init(struct action_interval* self, float duration)
{
    self->duration = duration;
    self->current = 0;
}

bool action_interval_update(struct action_interval* self, float dt)
{
    float next = self->current + dt;
    if (next > self->duration) {
        action_stop((struct action*)&self); // this conversion is safe. :)
        return true;
    }
    self->current = next;
    return false;
}

static bool action_move_update(struct action* self, struct sprite* sprite, float dt)
{
    struct action_move* move = &self->__internal.action_move;
    struct action_interval* super = &move->__super;

    float ratio = super->current / super->duration;
    float x = (move->to_x - move->from_x) * ratio;
    float y = (move->to_y - move->from_y) * ratio;
    sprite_set_pos(sprite, move->from_x + x, move->from_y + y);
    return action_interval_update(super, dt);
}

static bool action_scale_update(struct action* self, struct sprite* sprite, float dt)
{
    struct action_scale* scale = &self->__internal.action_sacle;
    struct action_interval* super = &scale->__super;

    float ratio = super->current / super->duration;
    float x = (scale->to_x - scale->from_x) * ratio;
    float y = (scale->to_y - scale->from_y) * ratio;
    sprite_set_scale_x(sprite, scale->from_x + x);
    sprite_set_scale_y(sprite, scale->from_y + y);

    return action_interval_update(super, dt);
}

static bool action_fade_update(struct action* self, struct sprite* sprite, float dt)
{
    struct action_fade_to* fade = &self->__internal.action_fade_to;
    struct action_interval* super = &fade->__super;

    float ratio = super->current / super->duration;
    unsigned char opacity = (fade->to - fade->from) * ratio;
    sprite_set_opacity(sprite, fade->from + opacity);

    return action_interval_update(super, dt);
}

static bool action_ease_in_update(struct action* self, struct sprite* sprite, float dt)
{
    struct action_ease_in* ease_in = &self->__internal.action_ease_in;
    struct action_interval* super = &ease_in->__super;

    float time = powf(super->current, ease_in->rate);
    action_update(ease_in->wrapped, sprite, time);

    return action_interval_update(super, dt);
}

static bool action_sequence_update(struct action* self, struct sprite* sprite, float dt)
{
//    struct action_sequence* seq = self->__child;
//
//    struct array* action_seq = seq->seqence;
//    int running_index = seq->running_index;
//    struct action* running = array_at(action_seq, running_index);
//    if(action_update(running, sprite, dt)) {
//        // action finished
//        seq->running_index++;
//
//        if (seq->running_index < array_size(action_seq)) {
//            // play the next action
//            struct action* next = array_at(action_seq,
//                                           seq->running_index);
//            action_play(next, sprite);
//            return false;
//        } else {
//            // all the sequence has finished :)
//            action_stop(self);
//            return true;
//        }
//    } else {
//        // current action is running, pass
//        return false;
//    }
    return true;
}

static bool action_call_update(struct action* self, struct sprite* sprite, float dt)
{
    struct action_call_lua_func* call = &self->__internal.action_call;
    s_assert(call->lua_func > 0);
    seal_call_func(self, NULL, NULL, true);
    action_stop(self);
    return true;
}

typedef bool (*ACTION_UPDATE_FUNC)(struct action*, struct sprite*, float);
static bool(*action_update_func[ACTION_TYPE_MAX])(struct action*, struct sprite*, float) = {
    action_move_update,
    action_scale_update,
    action_fade_update,
    action_ease_in_update,
    action_sequence_update,
    action_call_update
};

bool action_update(struct action* self, struct sprite* sprite, float dt)
{
    if (action_is_stop(self)) {
        return true;
    }

    switch (self->type) {
        case ACTION_MOVE_TO:
            {
                struct action_move* data = (struct action_move*)self->data;
                struct action_interval* super = &data->__super;

                if (!action_interval_update(&data->__super, dt)) {
                    float dx = (data->to_x - data->start_x)*dt/super->duration;
                    float dy = (data->to_y - data->start_y)*dt/super->duration;
                    sprite_set_pos(sprite, sprite->x + dx, sprite->y + dy);
                } else {
                    action_stop(self);
                }
            }
            break;

        default:
            break;
    }
}

void action_free(struct action* action)
{
    s_free(action);
}

static unsigned long action_id_counter = 0;
static struct action* action_new(enum action_type type)
{
    struct action* action = STRUCT_NEW(action);
    action->__id = ++action_id_counter;
    action->type = type;
    action->state = ACTION_STATE_READY;
    return action;
}

struct action* move_to(float duration, float to_x, float to_y)
{
    struct action* action = action_new(ACTION_MOVE_TO);

    struct action_move* data = STRUCT_NEW(action_move);
    data->to_x = to_x;
    data->to_y = to_y;
    data->start_x = data->start_y = 0;
    data->__super.current = 0;
    data->__super.duration = duration;

    action->data = data;

    return action;
}

struct action* scale_to(float duration, float to_x, float to_y)
{
    struct action* scale = action_new(ACTION_SCALE_TO);
    struct action_scale* interal = &scale->__internal.action_sacle;

    action_interval_init(&(scale->__internal.action_sacle.__super), duration);
    interal->to_x = to_x;
    interal->to_y = to_y;
    interal->from_x = interal->from_y = 0;
    return scale;
}

struct action* fade_to(float duration, unsigned char to)
{
    struct action* fade = action_new(ACTION_FADE_TO);
    struct action_fade_to* interal = &fade->__internal.action_fade_to;

    action_interval_init(&(fade->__internal.action_fade_to.__super), duration);
    interal->from = 255;
    interal->to = to;
    return fade;
}

struct action* ease_in(struct action* action, float rate)
{
    struct action* ease_in = action_new(ACTION_EASE_IN);
    struct action_ease_in* interal = &ease_in->__internal.action_ease_in;

    action_interval_init(&ease_in->__internal.action_ease_in.__super,
                         ((struct action_interval*)action)->duration);
    interal->rate = rate;
    interal->wrapped = action;
    return ease_in;
}

struct action* sequence(struct array* actions)
{
    return NULL;
//    struct action_sequence* sequence = STRUCT_NEW(action_sequence);
//    sequence->seqence = actions;
//    sequence->running_index = -1;

//    return action_new(ACTION_SEQUENCE, sequence);
}

struct action* call_lua_func()
{
    return NULL;
//    struct action_call_lua_func* call = STRUCT_NEW(action_call_lua_func);
//    call->lua_func = -1;
//    return action_new(ACTION_CALL, call);
}

void action_play(struct action* self, struct sprite* target)
{
    enum action_state state = self->state;
    if (state == ACTION_STATE_READY || state == ACTION_STATE_STOPPED) {
        self->state = ACTION_STATE_RUNNING;
    } else {
        fprintf(stderr, "invalid state to play. state = %d\n", state);
        return;
    }

    switch (self->type) {
        case ACTION_MOVE_TO:
        {
            struct action_move* move = &self->__internal.action_move;

            move->from_x = target->x;
            move->from_y = target->y;
            break;
        }

        case ACTION_SCALE_TO:
        {
            struct action_scale* scale = &self->__internal.action_sacle;

            scale->from_x = target->scale_x;
            scale->from_y = target->scale_y;
            break;
        }

        case ACTION_FADE_TO:
        {
            struct action_fade_to* fade = &self->__internal.action_fade_to;

            fade->from = target->color & 0xff;
            break;

        }

        case ACTION_EASE_IN:
        {
            break;
        }

        case ACTION_SEQUENCE:
        {
//            struct action_sequence* sequence =
//                                (struct action_sequence*)self->__child;
//            sequence->running_index = 0;
//            struct action* running = array_at(sequence->seqence,
//                                              sequence->running_index);
//            action_play(running, target);
            break;
        }
        case ACTION_CALL:
        {
            break;
        }

        default:
            break;
    }
}

void action_stop(struct action* self)
{
    self->state = ACTION_STATE_STOPPED;

    switch (self->type) {
        case ACTION_SEQUENCE:
        {

            break;
        }

        default:
            break;
    }
}

bool action_is_stop(struct action* self)
{
    return self->state == ACTION_STATE_STOPPED;
}

void action_pause(struct action* self)
{
    if (self->state == ACTION_STATE_RUNNING) {
        self->state = ACTION_STATE_PAUSED;
    }
}

void action_resume(struct action* self)
{
    if (self->state == ACTION_STATE_PAUSED) {
        self->state = ACTION_STATE_RUNNING;
    }
}
