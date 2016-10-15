#include <math.h>

#include "seal.h"
#include "sprite.h"
#include "action.h"

void action_interval_init(struct action_interval* self, float duration)
{
    self->duration = duration;
    self->current = 0;
}

bool action_interval_update(struct action_interval* self, float dt)
{
    float next = self->current + dt;
    if (next >= self->duration) {
        return true;
    }
    self->current = next;
    return false;
}

bool action_update(struct action* self, struct sprite* sprite, float dt)
{
    if (action_is_stop(self)) {
        return true;
    }

    switch (self->type) {
        case ACTION_MOVE_TO:
        {
            struct action_move* move = self->__child;
            struct action_interval* super = &move->__super;

            float ratio = super->current / super->duration;
            if (!action_interval_update(super, dt)) {
                float x = (move->to_x - move->start_x) * ratio;
                float y = (move->to_y - move->start_y) * ratio;
                sprite_set_pos(sprite, move->start_x + x, move->start_y + y);
            } else {
                action_stop(self);
            }
            break;
        }

        case ACTION_EASE_IN:
        {
            struct action_ease_in* ease_in = self->__child;
            struct action_interval* super = &ease_in->__super;

            if (!action_interval_update(super, dt)) {
                float time = powf(super->current, ease_in->rate);
                action_update(ease_in->wrapped, sprite, time);
            } else {
                action_stop(self);
            }
            break;
        }

        case ACTION_SEQUENCE:
        {
            struct action_sequence* seq = self->__child;

            struct array* action_seq = seq->seqence;
            int running_index = seq->running_index;
            struct action* running = array_at(action_seq, running_index);
            if(action_update(running, sprite, dt)) {
                // action finished
                seq->running_index++;

                if (seq->running_index < array_size(action_seq)) {
                    // play the next action
                    struct action* next = array_at(action_seq,
                                                     seq->running_index);
                    action_play(next, sprite);
                } else {
                    // all the sequence has finished :)
                    action_stop(self);
                }
            } else {
                // current action is running, pass
            }
            break;
        }

        case ACTION_CALL:
        {
            struct action_call_lua_func* call = self->__child;
            s_assert(call->lua_func > 0);
            seal_call_func(self, NULL, NULL, true);
            action_stop(self);
            break;
        }

        default:
            break;
    }
    return false;
}

void action_free(struct action* action)
{
    s_free(action->__child);
    s_free(action);
}

static unsigned long action_id_counter = 0;
static struct action* action_new(enum action_type type, void* child)
{
    struct action* action = STRUCT_NEW(action);
    action->__id = ++action_id_counter;
    action->type = type;
    action->__child = child;
    action->state = ACTION_STATE_READY;
    return action;
}

struct action* move_to(float duration, float to_x, float to_y)
{
    struct action_move* move = STRUCT_NEW(action_move);
    action_interval_init(&move->__super, duration);
    move->to_x = to_x;
    move->to_y = to_y;
    move->start_x = move->start_y = 0;

    return action_new(ACTION_MOVE_TO, move);
}

struct action* ease_in(struct action* action, float rate)
{
    struct action_ease_in* ease_in = STRUCT_NEW(action_ease_in);
    action_interval_init(&ease_in->__super,
                         ((struct action_interval*)action->__child)->duration
                         );
    ease_in->rate = rate;
    ease_in->wrapped = action;

    return action_new(ACTION_EASE_IN, ease_in);
}

struct action* sequence(struct array* actions)
{
    struct action_sequence* sequence = STRUCT_NEW(action_sequence);
    sequence->seqence = actions;
    sequence->running_index = -1;

    return action_new(ACTION_SEQUENCE, sequence);
}

struct action* call_lua_func()
{
    struct action_call_lua_func* call = STRUCT_NEW(action_call_lua_func);
    call->lua_func = -1;
    return action_new(ACTION_CALL, call);
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
            struct action_move* move = self->__child;

            move->start_x = target->x;
            move->start_y = target->y;
            break;
        }

        case ACTION_EASE_IN:
        {
            break;
        }

        case ACTION_SEQUENCE:
        {
            struct action_sequence* sequence =
                                (struct action_sequence*)self->__child;
            sequence->running_index = 0;
            struct action* running = array_at(sequence->seqence,
                                              sequence->running_index);
            action_play(running, target);
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