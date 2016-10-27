#ifndef scheduler_h
#define scheduler_h

struct Hashmap;
struct array;
struct action;
struct sprite;

struct schedule_entry {
    struct sprite* target;
    struct action* action;
};

struct scheduler {
    struct Hashmap* entries;
    float dt;
    float time_scale;

    struct sprite* __removing_target;
    struct array* __entries_to_remove;
};

struct scheduler* scheduler_new();
void scheduler_free(struct scheduler* self);

void scheduler_schedule(struct scheduler* self,
                        struct sprite* sprite,
                        struct action* action);

void scheduler_stop_target(struct scheduler* self,
                           struct sprite* sprite);

void scheduler_update(struct scheduler* self, float dt);
void scheduler_pause(struct scheduler* self);
void scheduler_resume(struct scheduler* self);
void scheduler_set_time_scale(struct scheduler* self, float time_scale);

#endif
