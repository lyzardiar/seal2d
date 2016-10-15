#include <math.h>

#include "seal.h"
#include "sprite.h"
#include "action.h"

float easeIn(float time, float rate)
{
    return powf(time, rate);
}

float easeOut(float time, float rate)
{
    return powf(time, 1 / rate);
}

float easeInOut(float time, float rate)
{
    time *= 2;
    if (time < 1)
    {
        return 0.5f * powf(time, rate);
    }
    else
    {
        return (1.0f - 0.5f * powf(2 - time, rate));
    }
}



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

void action_update(struct action* self, struct sprite* sprite, float dt)
{
    if (action_is_stop(self)) {
        return;
    }

    switch (self->type) {
        case ACTION_MOVE_TO:
        {
            struct action_move* move = self->__child;
            struct action_interval* super = &move->__super;

            float ratio = super->current / super->duration;
            if (!action_interval_update(&move->__super, dt)) {
                float x = (move->to_x - move->start_x) * ratio;
                float y = (move->to_y - move->start_y) * ratio;
                sprite_set_pos(sprite, move->start_x + x, move->start_y + y);
            } else {
                action_stop(self);
            }
        }
            break;

        case ACTION_EASE_IN:
        {
            struct action_ease_in* ease_in = self->__child;
            float time = powf(dt, ease_in->rate);
            printf("dt = %.4f, time = %.4f\n", dt, time);
            action_update(ease_in->wrapped, sprite, time);

        }
            break;
            
        default:
            break;
    }
}

void action_free(struct action* action)
{
    s_free(action->__child);
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
    struct action_move* move = STRUCT_NEW(action_move);
    action_interval_init(&move->__super, duration);
    move->to_x = to_x;
    move->to_y = to_y;
    move->start_x = move->start_y = 0;

    struct action* act = action_new(ACTION_MOVE_TO);
    act->__child = move;
    
    return act;
}

struct action* ease_in(struct action* action, float rate)
{
    struct action_ease_in* ease_in = STRUCT_NEW(action_ease_in);
    action_interval_init(&ease_in->__super,
                         ((struct action_interval*)(action->__child))->duration
                         );
    ease_in->rate = rate;
    ease_in->wrapped = action;

    struct action* act = action_new(ACTION_EASE_IN);
    act->__child = ease_in;
    return act;
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
        }
            break;

        case ACTION_EASE_IN:
        {

        }
            break;
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