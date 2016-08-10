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
    ACTION_STATE_FINISHED,
};

struct action_manager {
    struct Hashmap* actions;
    
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

bool action_interval_update(struct action_interval* self);

struct action_move {
    struct action_interval __super;
    float dx, dy;
};

struct action* move_to(struct sprite* sprite, float target_x, float target_y, float duration);

void action_play(struct action* self);
void action_stop(struct action* self);
void action_pause(struct action* self);
void action_resume(struct action* self);

void action_update(struct action* self, struct sprite* sprite, float dt);
void action_free(struct action* self);

#endif
