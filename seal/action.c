#include "seal.h"
#include "sprite.h"
#include "action.h"

bool action_interval_update(struct action_interval* action) {
    return true;
}

void action_update(struct action* self, struct sprite* sprite, float dt) {
    switch (self->type) {
        case ACTION_MOVE_TO:
            {
                struct action_move* data = (struct action_move*)self->data;
                struct action_interval* super = &data->__super;

                if (!action_interval_update(&data->__super)) {
                    sprite_set_pos(sprite,
                                   sprite->x + data->dx*dt/super->duration,
                                   sprite->y + data->dy*dt/super->duration);
                } else {
                    action_stop(self);
                }
            }
            break;
            
        default:
            break;
    }
}

void action_free(struct action* action) {
    s_free(action->data);
    s_free(action);
}

struct action* move_to(struct sprite* sprite, float target_x, float target_y, float duration)
{
    struct action* action = STRUCT_NEW(action);
    action->type = ACTION_MOVE_TO;
    action->state = ACTION_STATE_READY;
    
    struct action_move* data = STRUCT_NEW(action_move);
    data->dx = target_x - sprite->x;
    data->dy = target_y - sprite->y;
    data->__super.current = 0;
    data->__super.duration = duration;
    action->data = data;
    
    return action;
}

void action_play(struct action* self)
{
    if (self->state == ACTION_STATE_READY || self->state == ACTION_STATE_FINISHED) {
        self->state = ACTION_STATE_RUNNING;
    } else {
        fprintf(stderr, "invalid state passed when try to play action. state = %d\n", self->state);
    }
}

void action_stop(struct action* self)
{
    self->state = ACTION_STATE_FINISHED;
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