/*
 * Copyright (C) 2016 Tang Yiyang
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See BELOW for details.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "../seal.h"

struct affine* af_alloc()
{
    struct affine* af = (struct affine*)s_malloc(sizeof(struct affine));
    if(!af) {
        fprintf(stderr, "malloc affine failed for oom.");
        return NULL;
    }
    af_identify(af);
    return af;
}

void af_free(struct affine* af)
{
    s_free(af);
}

void af_identify(struct affine* af)
{
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
            float rotation)
{
    struct affine scale_matrix;
    struct affine rotation_matrix;
    af_identify(&scale_matrix);
    af_identify(&rotation_matrix);

    af_set_scale(&scale_matrix, scale_x, scale_y);
    af_set_rotation(&rotation_matrix, rotation);

    af_copy(af, &scale_matrix);
    af_concat(af, &rotation_matrix);

    af->x += x;
    af->y += y;
}

void af_copy(struct affine* af, struct affine* other)
{
    memcpy(af, other, sizeof(struct affine));
}

void af_set_translate(struct affine* af, float x, float y)
{
    af->x = x;
    af->y = y;
}

void af_set_scale(struct affine* af, float scale_x, float scale_y)
{
    af->a = scale_x;
    af->d = scale_y;
}

void af_set_rotation(struct affine* af, float rotate)
{
    float sin = sinf(rotate*M_PI/180.0f);
    float cos = cosf(rotate*M_PI/180.0f);
    af->a = cos;
    af->b = -sin;
    af->c = sin;
    af->d = cos;
}

// [a1, b1, x1]   [a2, b2, x2]   [a1*a2+b1*c2, a1*b2+b1*d2, a1*x2+b1*y2+x1]
// [c1, d1, y1] * [c2, d2, y2] = [c1*a2+d1*c2, c1*b2+d1*d2, c1*x2+d1*y2+y1]
// [0,  0,  1]    [0,  0,  1]    [0,           0,           1             ]
void af_concat(struct affine* m1, struct affine* m2)
{
    float a = m1->a*m2->a + m1->b*m2->c;
    float b = m1->a*m2->b + m1->b*m2->d;
    float x = m1->x*m2->a + m1->y*m2->c + m2->x;

    float c = m1->c*m2->a + m1->d*m2->c;
    float d = m1->c*m2->b + m1->d*m2->d;
    float y = m1->x*m2->b + m1->y*m2->d + m2->y;

    m1->a = a;
    m1->b = b;
    m1->c = c;
    m1->d = d;
    m1->x = x;
    m1->y = y;
}

void af_tostring(struct affine* af, char* buff)
{
    sprintf(buff, "{a = %.2f, b = %.2f, c = %.2f, d = %.2f, x = %.2f, y = %.2f}",
            af->a, af->b, af->c, af->d, af->x, af->y);
}
