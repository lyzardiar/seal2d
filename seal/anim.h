//
//  anim.h
//  yuusha
//
//  Created by 艺洋 唐 on 6/21/16.
//  Copyright © 2016 levelmax. All rights reserved.
//

#ifndef anim_h
#define anim_h

#include "array.h"

struct sprite_frame;

struct anim {
    unsigned int __id;
    char* anim_name;
    float interval;
    void (*callback)(struct anim*);
    struct array* sprite_frames;
};

struct anim* anim_new(const char* file);
void anim_free(struct anim* self);

void anim_play(struct anim* self);
void anim_stop(struct anim* self);
void anim_resume(struct anim* self);
void anim_set_speed(struct anim* self, float scalar);
void anim_set_callback(struct anim* self, void (*callback)(struct anim*) );

struct sprite_frame* anim_frame(struct anim* self, unsigned int frame_index);



#endif /* anim_h */
