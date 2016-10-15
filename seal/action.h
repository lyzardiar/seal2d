#ifndef __seal__action__
#define __seal__action__

struct sprite;
struct Hashmap;
struct array;

enum action_type {
    ACTION_MOVE_TO = 0,
    ACTION_EASE_IN,
    ACTION_SEQUENCE,
};

enum action_state {
    ACTION_STATE_READY = 0,
    ACTION_STATE_RUNNING,
    ACTION_STATE_PAUSED,
    ACTION_STATE_STOPPED,
};

struct action {
    unsigned long __id;
    enum action_type type;
    enum action_state state;
    void* __child;
};

struct action_interval {
    struct action __super;
    float current;
    float duration;
};

struct action_move {
    struct action_interval __super;
    float to_x, to_y;
    float start_x, start_y;
};

struct action_ease_in {
    struct action_interval __super;
    float rate;
    struct action* wrapped;
};

struct action_sequence {
    struct array* seqence; // sequence of actions
    int running_index;
};

void action_interval_init(struct action_interval* self, float duration);
bool action_interval_update(struct action_interval* self, float dt);

struct action* move_to(float duration, float to_x, float to_y);
struct action* ease_in(struct action* action, float rate);
struct action* sequence(struct array* actions);

void action_play(struct action* self, struct sprite* target);
void action_stop(struct action* self);
void action_pause(struct action* self);
void action_resume(struct action* self);

bool action_is_stop(struct action* self);

bool action_update(struct action* self, struct sprite* sprite, float dt);
void action_free(struct action* self);

#endif
