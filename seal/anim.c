#include <math.h>
#include <float.h>
#include "memory.h"

#include "anim.h"

static unsigned int __anim_id = 0;
struct anim* anim_new(struct array* sprite_frames) {
    struct anim* anim = STRUCT_NEW(anim);
    
    anim->interval = 1.0f/30.0f;
    anim->callback = NULL;
    anim->sprite_frames = array_copy(sprite_frames);
    anim->speed = 1.0f;
    
    // privates
    anim->__id = ++__anim_id;
    anim->__now = 0.0f;
    anim->__cur_frame = 0;
    anim->__total_frame = array_size(anim->sprite_frames);
    anim->__state = ANIM_STATE_PLAY;
    return anim;
}

void anim_free(struct anim* self) {
    array_free(self->sprite_frames);
    s_free(self);
}

void anim_update(struct anim* self, float dt) {
    if(self->__state != ANIM_STATE_PLAY) {
        return;
    }
    
    float next = self->__now + dt * self->speed;
    if(next >= self->interval) {
        self->__now = 0.0f;
        self->__cur_frame = (++self->__cur_frame) % self->__total_frame;
    } else {
        self->__now = next;
    }
}

void anim_play(struct anim* self) {
    if(self->__state == ANIM_STATE_STOPPED) {
        self->__state = ANIM_STATE_PLAY;
    }
}

void anim_stop(struct anim* self) {
    if(self->__state != ANIM_STATE_STOPPED) {
        self->__state = ANIM_STATE_STOPPED;
    }
}

void anim_resume(struct anim* self) {
    if(self->__state == ANIM_STATE_PAUSED) {
        self->__state = ANIM_STATE_PLAY;
    }
}

void anim_set_speed(struct anim* self, float speed) {
    self->speed = speed;
}

void anim_set_callback(struct anim* self, void (*callback)(struct anim*) ) {
    self->callback = callback;
}

struct sprite_frame* anim_current_frame(struct anim* self) {
    return anim_frame(self, self->__cur_frame);
}

struct sprite_frame* anim_frame(struct anim* self, unsigned int frame_index) {
    return array_at(self->sprite_frames, frame_index);
}

