#include "seal.h"
#include "sprite.h"
#include "action.h"

void action_update(struct action* action, struct sprite* sprite, float dt) {
    switch (action->type) {
        case ACTION_MOVE_TO:
            {
                struct action_move* data = (struct action_move*)action->data;
                sprite_set_pos(sprite,
                               sprite->local_srt.x + data->dx*dt/data->duration,
                               sprite->local_srt.y + data->dy*dt/data->duration);
                data->current += dt;
                if (data->current >= data->duration) {
                    
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
    data->current = 0;
    action->data = data;
    
    return action;
}