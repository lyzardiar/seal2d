#include <math.h>

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
    
    af->c = 1.0f;
    af->d = 0.0f;
    af->y = 0.0f;
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
    m1->a = m1->a*m2->a + m1->b*m2->c;
    m1->b = m1->a*m2->b + m1->b*m2->d;
    m1->x = m1->a*m2->x + m1->b*m2->y + m1->x;
    
    m1->c = m1->c*m2->a + m1->d*m2->c;
    m1->d = m1->c*m2->b + m1->d*m2->d;
    m1->y = m1->c*m2->x + m1->d*m2->y + m1->y;
}