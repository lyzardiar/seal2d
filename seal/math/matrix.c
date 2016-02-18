#include "matrix.h"

// [0,0] [0,1] [0,2] [0,3]
// [1,0] [1,1] [1,2] [1,3]
// [2,0] [2,1] [2,2] [2,3]
// [3,0] [3,1] [3,2] [3,3]

struct mat4* orth_matrix(float left, float bottom, float right, float top, float near, float far) {
    struct mat4* mat = (struct mat4*)s_malloc(sizeof(struct mat4));
    float* m = mat->m;
    m[0] = 2/(right - left);
    m[1] = 0;
    m[2] = 0;
    m[3] = -(right+left)/(right-left);
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
    
    debug_print(mat);
    return mat;
}

struct mat4* identy_matix() {
    struct mat4* mat = (struct mat4*)s_malloc(sizeof(struct mat4));
    float* m = mat->m;
    m[0] = 1.0f;
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;
    m[4] = 0;
    m[5] = 1.0f;
    m[6] = 0;
    m[7] = 0;
    m[8] = 0;
    m[9] = 0;
    m[10] = 1.0f;
    m[11] = 0.0f;
    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 1.0f;
    
    debug_print(mat);
    return mat;
}

void matrix_free(struct mat4* mat) {
    s_free(mat);
}

void matrix_translate(struct mat4* self, struct mat4* out, float x, float y, float z) {
    float* im = self->m;
    float* om = out->m;
    
    om[0] = im[0] * x;
    om[3] = im[3];
    
    om[5] = im[5] * y;
    om[7] = im[7];
    
    om[10] = im[10] * z;
    om[11] = im[11];
    
    debug_print(out);
}

void debug_print(struct mat4* self) {
    float* m = self->m;
    printf("--------------------------\n");
    printf("|\t %.5f,\t %.5f,\t %.5f,\t %.5f\t |\n", m[0], m[1], m[2], m[3]);
    printf("|\t %.5f,\t %.5f,\t %.5f,\t %.5f\t |\n", m[4], m[5], m[6], m[7]);
    printf("|\t %.5f,\t %.5f,\t %.5f,\t %.5f\t |\n", m[8], m[9], m[10], m[11]);
    printf("|\t %.5f,\t %.5f,\t %.5f,\t %.5f\t |\n", m[12], m[13], m[13], m[14]);
    printf("--------------------------\n");
}