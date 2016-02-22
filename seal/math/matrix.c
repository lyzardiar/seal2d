#include "matrix.h"

//0,4,8,12,
//1,5,9,13,
//2,6,10,14,
//3,7,11,15
struct mat4* orth_matrix(float left, float bottom, float right, float top, float near, float far) {
    struct mat4* mat = (struct mat4*)s_malloc(sizeof(struct mat4));
    float* m = mat->m;
    m[0] = 2.0f / (right - left);
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = 0.0f;
    m[4] = 0.0f;
    m[5] = 2.0f / (top - bottom);
    m[6] = 0.0f;
    m[7] = 0.0f;
    m[8] = 0.0f;
    m[9] = 0.0f;
    m[10] = 2.0f / (near - far);
    m[11] = 0.0f;
    m[12] = (left + right) / (left - right);
    m[13] = -(top + bottom) / (top - bottom);
    m[14] = (near + far) / (near - far);
    m[15] = 1.0f;
    return mat;
}

struct mat4* identy_matix() {
    struct mat4* mat = (struct mat4*)s_malloc(sizeof(struct mat4));
    float* m = mat->m;
    m[0] = 1.0f;
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = 0.0f;
    m[4] = 0.0f;
    m[5] = 1.0f;
    m[6] = 0.0f;
    m[7] = 0.0f;
    m[8] = 0.0f;
    m[9] = 0.0f;
    m[10] = 1.0f;
    m[11] = 0.0f;
    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = 0.0f;
    m[15] = 1.0f;
    
    return mat;
}

void matrix_free(struct mat4* mat) {
    s_free(mat);
}

void matrix_translate(struct mat4* self, float x, float y, float z) {
    float* om = self->m;
    
    om[12] += x;
    om[13] += y;
    om[14] += z;
}

void matrix_debug_print(struct mat4* self) {
    float* m = self->m;
    printf("------------------------------------------------\n");
    printf("|\t %.5f,\t %.5f,\t %.5f,\t %.5f\t |\n", m[0], m[4], m[8], m[12]);
    printf("|\t %.5f,\t %.5f,\t %.5f,\t %.5f\t |\n", m[1], m[5], m[9], m[13]);
    printf("|\t %.5f,\t %.5f,\t %.5f,\t %.5f\t |\n", m[2], m[6], m[10], m[14]);
    printf("|\t %.5f,\t %.5f,\t %.5f,\t %.5f\t |\n", m[3], m[7], m[11], m[15]);
    printf("------------------------------------------------\n");
}