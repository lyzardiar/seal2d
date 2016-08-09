#include "seal.h"
#include "sprite.h"
#include "action.h"

bool action_interval_update(struct action_interval* action) {
    return true;
}

void action_update(struct action* action, struct sprite* sprite, float dt) {
    switch (action->type) {
        case ACTION_MOVE_TO:
            {
                struct action_move* data = (struct action_move*)action->data;
                struct action_interval* super = &data->__super;

                if (!action_interval_update(&data->__super)) {
                    sprite_set_pos(sprite,
                                   sprite->local_srt.x + data->dx*dt/super->duration,
                                   sprite->local_srt.y + data->dy*dt/super->duration);
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

struct action* move_to(struct sprite* sprite, float target_x, float target_y, float duration) {
    struct action* action = STRUCT_NEW(action);
    action->type = ACTION_MOVE_TO;
    
    struct action_move* data = STRUCT_NEW(action_move);
    data->dx = target_x - sprite->local_srt.x;
    data->dy = target_y - sprite->local_srt.y;
    data->__super.current = 0;
    data->__super.duration = duration;
    action->data = data;
    
    return action;
}