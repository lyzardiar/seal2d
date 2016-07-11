#include "geo.h"

bool rect_contains(struct rect* rect, float x, float y) {
    return !(rect->x                 > x ||
             rect->x+rect->width     < x ||
             rect->y                 > y ||
             rect->y + rect->height  < y );
}

void color_vec4(unsigned long color, float v[4]) {
    v[0] = ((color >> 24) & 0xff) / 255.0;
    v[1] = ((color >> 16) & 0xff) / 255.0;
    v[2] = ((color >> 8 ) & 0xff) / 255.0;
    v[3] = ((color      ) & 0xff) / 255.0;
}

void color_c4f(unsigned long color, float* r, float* g, float* b, float* a) {
    *r = ((color >> 24) & 0xff) / 255.0;
    *g = ((color >> 16) & 0xff) / 255.0;
    *b = ((color >> 8 ) & 0xff) / 255.0;
    *a = ((color      ) & 0xff) / 255.0;
}