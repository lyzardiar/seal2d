#ifndef __yuusha__camera__
#define __yuusha__camera__

#include "seal.h"

struct camera {
    float x, y;
    float width, height;
    float scale;
    struct mat4* mat;
    int dirty;
};

struct camera* camera_new(float width, float height);
void camera_update(struct camera* c);
#endif /* defined(__yuusha__camera__) */
