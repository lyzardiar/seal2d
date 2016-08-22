#ifndef __seal__action__
#define __seal__action__

struct sprite;
struct Hashmap;

enum action_type {
    ACTION_MOVE_TO = 0,
};

enum action_state {
    ACTION_STATE_READY = 0,
    ACTION_STATE_RUNNING,
    ACTION_STATE_PAUSED,
    ACTION_STATE_STOPPED,
};

struct action {
    enum action_type type;
    enum action_state state;
    void* data;
};

struct action_interval {
    float current;
    float duration;
};

bool action_interval_update(struct action_interval* self, float dt);

struct action_move {
    struct action_interval __super;
    float to_x, to_y;
    float start_x, start_y;
};

struct action* move_to(float duration, float to_x, float to_y);

void action_play(struct action* self, struct sprite* target);
void action_stop(struct action* self);
void action_pause(struct action* self);
void action_resume(struct action* self);

bool action_is_stop(struct action* self);

void action_update(struct action* self, struct sprite* sprite, float dt);
void action_free(struct action* self);

#endif
