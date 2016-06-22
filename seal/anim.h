#ifndef anim_h
#define anim_h

#include "array.h"

struct sprite_frame;

#define MAX_FRAME (60)

struct anim {
    unsigned int __id;
    float interval;
    void (*callback)(struct anim*);
    
    struct array* sprite_frames; // only have a reference of the frames
};

struct anim* anim_new(struct array* sprite_frames);
void anim_free(struct anim* self);

void anim_update(struct anim* self, float dt);
void anim_play(struct anim* self);
void anim_stop(struct anim* self);
void anim_resume(struct anim* self);
void anim_set_speed(struct anim* self, float scalar);
void anim_set_callback(struct anim* self, void (*callback)(struct anim*) );

struct sprite_frame* anim_frame(struct anim* self, unsigned int frame_index);

#endif