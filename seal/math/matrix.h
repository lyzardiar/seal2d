#ifndef __yuusha__matrix__
#define __yuusha__matrix__

#include "seal.h"

struct mat4 {
    float m[16];
};

struct mat4* orth_matrix(float left, float bottom, float right, float top, float near, float far);

#endif /* defined(__yuusha__matrix__) */
