#ifndef anim_h
#define anim_h

#include "array.h"

struct sprite_frame;

#define MAX_FRAME (60)

#define ANIM_STATE_PLAY 0
#define ANIM_STATE_PAUSED 1
#define ANIM_STATE_STOPPED 2

struct anim {
    unsigned int __id;
    unsigned int __cur_frame;
    unsigned int __total_frame;
    int __state;
    float __now;

    struct array* sprite_frames; // only have a reference of the frames
    float interval;
    float speed;
    void (*callback)(struct anim*);
};

struct anim* anim_new(struct array* sprite_frames);
void anim_free(struct anim* self);

void anim_update(struct anim* self, float dt);
void anim_play(struct anim* self);
void anim_stop(struct anim* self);
void anim_resume(struct anim* self);
void anim_set_speed(struct anim* self, float speed);
void anim_set_callback(struct anim* self, void (*callback)(struct anim*) );

struct sprite_frame* anim_current_frame(struct anim* self);
struct sprite_frame* anim_frame(struct anim* self, unsigned int frame_index);

#endif