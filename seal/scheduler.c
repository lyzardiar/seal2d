#include "seal.h"
#include "scheduler.h"

#include "hashmap.h"
#include "action.h"

static int hash_str(void* key)
{
    return hashmapHash(key, sizeof(void*));
}

static bool hash_equal(void* a, void* b)
{
    return a == b;
}

struct scheduler* scheduler_new()
{
    struct scheduler* s = STRUCT_NEW(scheduler);
    s->entries = hashmapCreate(32, hash_str, hash_equal);
    s->time_scale = 1.0f;
    return s;
}

void scheduler_free(struct scheduler* self)
{
    hashmapFree(self->entries);
    s_free(self);
}

void scheduler_schedule(struct scheduler* self,
                        struct sprite* sprite,
                        struct action* action)
{
    action_play(action, sprite);

    struct schedule_entry* entry = STRUCT_NEW(schedule_entry);
    entry->action = action;
    entry->target = sprite;
    hashmapPut(self->entries, entry->action, entry);
}

static bool scheduler_entries_update(void* key, void* value, void* context)
{
    struct schedule_entry* entry = (struct schedule_entry*)value;
    struct scheduler* self = (struct scheduler*)context;
    struct action* act = entry->action;
    action_update(act, entry->target, self->dt);
    if (action_is_stop(act)) {
        hashmapRemove(self->entries, act);
        action_free(act);
    }
    return true;
}

void scheduler_update(struct scheduler* self, float dt)
{
    self->dt = dt * self->time_scale;
    hashmapForEach(self->entries, scheduler_entries_update, self);
}

void scheduler_pause(struct scheduler* self)
{

}

void scheduler_resume(struct scheduler* self)
{

}

void scheduler_set_time_scale(struct scheduler* self, float time_scale)
{
    self->time_scale = time_scale;
}
