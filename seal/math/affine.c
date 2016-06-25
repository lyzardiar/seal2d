#include <math.h>
#include <float.h>

#include "affine.h"

struct affine* af_alloc() {
    struct affine* af = (struct affine*)s_malloc(sizeof(struct affine));
    if(!af) {
        fprintf(stderr, "malloc affine failed for oom.");
        return NULL;
    }
    af_identify(af);
    return af;
}

void af_free(struct affine* af) {
    s_free(af);
}

void af_identify(struct affine* af) {
    af->a = 1.0f;
    af->b = 0.0f;
    af->x = 0.0f;
    
    af->c = 0.0f;
    af->d = 1.0f;
    af->y = 0.0f;
}

void af_srt(struct affine* af,
            float x, float y,
            float scale_x, float scale_y,
            float rotation) {
    af_identify(af);
    struct affine scale_matrix;
    struct affine rotation_matrix;
    struct affine translate_matrix;
    af_identify(&scale_matrix);
    af_identify(&rotation_matrix);
    af_identify(&translate_matrix);
    
    af_set_scale(&scale_matrix, scale_x, scale_y);
    af_set_rotation(&rotation_matrix, rotation);
    af_set_translate(&translate_matrix, x, y);
    
    af_concat(af, &scale_matrix);
    af_concat(af, &rotation_matrix);
    af_concat(af, &translate_matrix);
}


void af_set_translate(struct affine* af, float x, float y) {
    af->x = x;
    af->y = y;
}

void af_set_scale(struct affine* af, float scale_x, float scale_y) {
    af->a = scale_x;
    af->d = scale_y;
}

void af_set_rotation(struct affine* af, float rotate) {
    float sin = sinf(rotate*M_PI/180.0f);
    float cos = cosf(rotate*M_PI/180.0f);
    af->a = cos;
    af->b = -sin;
    af->c = sin;
    af->d = cos;
    
}

void af_translate(struct affine* af, float x, float y) {
    af->x += x;
    af->y += y;
}

void af_scale(struct affine* af, float scale_x, float scale_y) {
    af->a *= scale_x;
    af->b *= scale_y;
    af->c *= scale_x;
    af->d *= scale_y;
}

void af_rotate(struct affine* af, float rotate) {
    float sin = sinf(rotate);
    float cos = cosf(rotate);
    af->a *= cos;
    af->b *= -sin;
    af->c *= sin;
    af->d *= cos;
}

// [a1, b1, x1]   [a2, b2, x2]   [a1*a2+b1*c2, a1*b2+b1*d2, a1*x2+b1*y2+x1]
// [c1, d1, y1] * [c2, d2, y2] = [c1*a2+d1*c2, c1*b2+d1*d2, c1*x2+d1*y2+y1]
// [0,  0,  1]    [0,  0,  1]    [0,           0,           1             ]

void af_concat(struct affine* m1, struct affine* m2) {
    float a = m1->a*m2->a + m1->b*m2->c;
    float b = m1->a*m2->b + m1->b*m2->d;
    float x = m1->a*m2->x + m1->b*m2->y + m1->x;
    
    float c = m1->c*m2->a + m1->d*m2->c;
    float d = m1->c*m2->b + m1->d*m2->d;
    float y = m1->c*m2->y + m1->d*m2->y + m1->y;
    
    m1->a = a;
    m1->b = b;
    m1->c = c;
    m1->d = d;
    m1->x = x;
    m1->y = y;
}

//    [a, b, x]   [x']    [a*x' + b*y' + x]
//    [c, d, y] * [y'] =  [c*x' + d*y' + y]
//    [0, 0, 1]   [1]     [1]
void af_mul(struct affine* af, float *x, float *y) {
    float rx = af->a*(*x) + af->b*(*y) + af->x;
    float ry = af->c*(*x) + af->d*(*y) + af->y;
    *x = rx;
    *y = ry;
}

void af_tostring(struct affine* af, char* buff) {
    sprintf(buff, "{a = %.2f, b = %.2f, c = %.2f, d = %.2f, x = %.2f, y = %.2f}",
            af->a, af->b, af->c, af->d, af->x, af->y);
}