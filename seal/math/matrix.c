#include "matrix.h"

// [0,0] [0,1] [0,2] [0,3]
// [1,0] [1,1] [1,2] [1,3]
// [2,0] [2,1] [2,2] [2,3]
// [3,0] [3,1] [3,2] [3,3]

struct mat4* orth_matrix(float left, float bottom, float right, float top, float near, float far) {
    struct mat4* m = (struct mat4*)s_malloc(sizeof(struct mat4));
    m[0] = 2/(right - left);
    m[1] = 0;
    m[2] = 0;
    m[3] = -(right+left)/(right-left)
    m[4] = 0;
    m[5] = 2/(top-bottom);
    m[6] = 0;
    m[7] = -(top+bottom)/(top-bottom);
    m[8] = 0;
    m[9] = 0;
    m[10] = -2/(far-near);
    m[11] = -(far + near) / (far - near);
    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 1;
    return m;
}