#include "seal.h"
#include "sprite.h"
#include "action.h"

// return value: true means this action is already finished.
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
    s_free(action->data);
    s_free(action);
}

struct action* move_to(float duration, float to_x, float to_y)
{
    struct action* action = STRUCT_NEW(action);
    action->type = ACTION_MOVE_TO;
    action->state = ACTION_STATE_READY;
    
    struct action_move* data = STRUCT_NEW(action_move);
    data->to_x = to_x;
    data->to_y = to_y;
    data->start_x = data->start_y = 0;
    data->__super.current = 0;
    data->__super.duration = duration;
    action->data = data;
    
    return action;
}

void action_play(struct action* self, struct sprite* target)
{
    if (self->state == ACTION_STATE_READY || self->state == ACTION_STATE_STOPPED) {
        self->state = ACTION_STATE_RUNNING;
    } else {
        fprintf(stderr, "invalid state passed when try to play action. state = %d\n", self->state);
    }
    switch (self->type) {
        case ACTION_MOVE_TO:
        {
            struct action_move* move = self->data;
            move->start_x = target->x;
            move->start_y = target->y;
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