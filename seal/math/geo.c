#include <stdio.h>
#include "geo.h"

bool rect_contains(struct rect* rect, float x, float y)
{
    return !(rect->x                 > x ||
             rect->x+rect->width     < x ||
             rect->y                 > y ||
             rect->y + rect->height  < y );
}

void color_vec4(unsigned long color, float v[4])
{
    v[0] = ((color >> 24) & 0xff) / 255.0f;
    v[1] = ((color >> 16) & 0xff) / 255.0f;
    v[2] = ((color >> 8 ) & 0xff) / 255.0f;
    v[3] = ((color      ) & 0xff) / 255.0f;
}

void color_c4f(unsigned long color, float* r, float* g, float* b, float* a)
{
    *r = ((color >> 24) & 0xff) / 255.0f;
    *g = ((color >> 16) & 0xff) / 255.0f;
    *b = ((color >> 8 ) & 0xff) / 255.0f;
    *a = ((color      ) & 0xff) / 255.0f;
}

void vertex_tostring(char* buff, struct vertex* vertex)
{
    snprintf(buff, 1024, "vertex (%p) { position = {%.2f, %.2f}, color = {%d, %d, %d, %d} }",
                vertex,
                vertex->position[0], vertex->position[1],
                vertex->color[0], vertex->color[1], vertex->color[2], vertex->color[3] );
}