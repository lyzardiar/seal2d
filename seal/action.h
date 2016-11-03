#ifndef __seal__action__
#define __seal__action__

#include "seal_base.h"

struct sprite;
struct Hashmap;
struct array;

enum action_type {
    ACTION_MOVE_TO = 0,
    ACTION_SCALE_TO,
    ACTION_FADE_TO,
    ACTION_EASE_IN,
    ACTION_SEQUENCE,
    ACTION_CALL,
    ACTION_TYPE_MAX,
};

enum action_state {
    ACTION_STATE_READY = 0,
    ACTION_STATE_RUNNING,
    ACTION_STATE_PAUSED,
    ACTION_STATE_STOPPED,
};

#define ACTION_SEQUENCE_MAX (8)
struct action_interval {
    float current;
    float duration;
};

struct action_move {
    struct action_interval __super;
    float from_x, from_y;
    float to_x, to_y;
};

struct action_scale {
    struct action_interval __super;
    float from_x, from_y;
    float to_x, to_y;
};

struct action_fade_to {
    struct action_interval __super;
    unsigned char from, to;
};

struct action_ease_in {
    struct action_interval __super;
    float rate;
    struct action* wrapped;
};

struct action_sequence {
    void* sequence[ACTION_SEQUENCE_MAX];
    int running_index;
};

struct action_call_lua_func {
    LUA_FUNCTION_HANDLER lua_func;
};

struct action {
    unsigned long __id;
    enum action_type type;
    enum action_state state;
    union action_data {
        struct action_move action_move;
        struct action_scale action_sacle;
        struct action_fade_to action_fade_to;
        struct action_ease_in action_ease_in;
        struct action_sequence action_sequence;
        struct action_call_lua_func action_call;
    } __internal;
};


void action_interval_init(struct action_interval* self, float duration);
bool action_interval_update(struct action_interval* self, float dt);

struct action* move_to(float duration, float to_x, float to_y);
struct action* scale_to(float duration, float to_x, float to_y);
struct action* fade_to(float duration, unsigned char to);
struct action* ease_in(struct action* action, float rate);
struct action* sequence(struct array* actions);
struct action* call_lua_func();

void action_play(struct action* self, struct sprite* target);
void action_stop(struct action* self);
void action_pause(struct action* self);
void action_resume(struct action* self);

bool action_is_stop(struct action* self);

bool action_update(struct action* self, struct sprite* sprite, float dt);
void action_free(struct action* self);

#endif
