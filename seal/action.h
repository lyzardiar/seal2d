#ifndef __seal__action__
#define __seal__action__

#define ACTION_MOVE_TO (0)

struct sprite;

enum action_state {
    ACTION_STATE_READY = 0,
    ACTION_STATE_RUNNING,
    ACTION_STATE_PAUSED,
    ACTION_STATE_FINISHED,
};

struct action {
    int type;
    enum action_state state;
    void* data;
};

struct action_interval {
    float current;
    float duration;
};

bool action_interval_update(struct action_interval* action);

struct action_move {
    struct action_interval __super;
    float dx, dy;
};

struct action* move_to(struct sprite* sprite, float target_x, float target_y, float duration);

void action_update(struct action* action, struct sprite* sprite, float dt);
void action_free(struct action* action);

#endif
