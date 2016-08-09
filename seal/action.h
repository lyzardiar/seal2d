#ifndef __seal__action__
#define __seal__action__

#define ACTION_MOVE_TO (0)

struct sprite;

struct action {
    int type;
    void* data;
};

struct action_move {
    float dx, dy;
    float current;
    float duration;
};

struct action* move_to(struct sprite* sprite, float target_x, float target_y, float duration);

void action_update(struct action* action, struct sprite* sprite, float dt);
void action_free(struct action* action);

#endif
