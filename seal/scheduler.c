/*
 * Copyright (C) 2016 Tang Yiyang
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See BELOW for details.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "seal.h"


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

    s->__removing_target = NULL;
    s->__entries_to_remove = array_new(16);
    return s;
}

void scheduler_free(struct scheduler* self)
{
    hashmapFree(self->entries);
    array_free(self->__entries_to_remove);
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

static bool scheduler_stop_target_foreach(void* key, void* value, void* context)
{
    struct scheduler* self = (struct scheduler*)context;
    struct schedule_entry* entry = value;
    if (entry->target == self->__removing_target) {
        printf("try to remove target = %p for entry = %p\n", self->__removing_target, entry);
        array_push_back(self->__entries_to_remove, entry);
    }
    return true;
}

void scheduler_stop_target(struct scheduler* self,
                           struct sprite* sprite)
{
    // TODO: we could easily improve here by import another hash-map which
    // make sprite as the key before we do the actual iteration
    self->__removing_target = sprite;
    hashmapForEach(self->entries, scheduler_stop_target_foreach, self);
}

static bool scheduler_entries_update(void* key, void* value, void* context)
{
    struct schedule_entry* entry = (struct schedule_entry*)value;
    struct scheduler* self = (struct scheduler*)context;
    struct action* act = entry->action;
    if(action_update(act, entry->target, self->dt)) {
        array_push_back(self->__entries_to_remove, entry);
    }

    return true;
}

void scheduler_update(struct scheduler* self, float dt)
{
    self->dt = dt * self->time_scale;
    hashmapForEach(self->entries, scheduler_entries_update, self);

    struct array* to_remove = self->__entries_to_remove;
    int n = array_size(to_remove);
    if (n > 0) {
        for (int i = 0; i < n; ++i) {
            struct schedule_entry* e = array_at(to_remove, i);
            printf("free action = %p, id = %ld\n", e->action, e->action->__id);
            hashmapRemove(self->entries, e->action);
            action_free(e->action);
        }
        array_clear(self->__entries_to_remove, false);
        printf("after remove n = %d, total_action = %ld\n", n, hashmapSize(self->entries));
    }
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
