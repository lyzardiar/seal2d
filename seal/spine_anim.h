#ifndef spine_anim_h
#define spine_anim_h

#include "spine/spine.h"

struct spine_anim {
    spAtlas* atlas;
    spAnimationState* anim_state;
    spSkeleton* skeleton;

};

struct spine_anim* spine_anim_new(const char* atlas_path, const char* spine_data_path, float scale);
void spine_anim_free(struct spine_anim* self);

void spine_anim_update(struct spine_anim* self, float dt);
void spine_anim_draw(struct spine_anim* self);
void spine_anim_set_anim(struct spine_anim* self, const char* anim_name);

void spine_get_boundingbox(struct spine_anim* self, struct rect* r);

#endif
