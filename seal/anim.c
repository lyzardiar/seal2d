#include "memory.h"

#include "anim.h"

static unsigned int __anim_id = 0;
struct anim* anim_new(struct array* sprite_frames) {
    struct anim* anim = STRUCT_NEW(anim);
    
    anim->sprite_frames = sprite_frames;
    anim->interval = 1.0f/30.0f;
    anim->__id = ++__anim_id;
    anim->callback = NULL;
    return anim;
}

void anim_free(struct anim* self) {
    
}

void anim_update(struct anim* self, float dt) {
    
}

void anim_play(struct anim* self) {
    
}

void anim_stop(struct anim* self) {
    
}

void anim_resume(struct anim* self) {
    
}

void anim_set_speed(struct anim* self, float scalar) {
    
}
void anim_set_callback(struct anim* self, void (*callback)(struct anim*) ) {
    
}