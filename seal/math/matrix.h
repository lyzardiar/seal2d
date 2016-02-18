#ifndef __yuusha__matrix__
#define __yuusha__matrix__

#include "seal.h"

struct mat4 {
    float m[16];
};

struct mat4* identy_matix();
struct mat4* orth_matrix(float left, float bottom, float right, float top, float near, float far);

void matrix_free(struct mat4* mat);

void matrix_translate(struct mat4* self, struct mat4* out, float x, float y, float z);

void debug_print(struct mat4* self);
#endif /* defined(__yuusha__matrix__) */
