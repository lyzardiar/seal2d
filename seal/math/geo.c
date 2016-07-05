#include "geo.h"

bool rect_contains(struct rect* rect, float x, float y) {
    return !(rect->x                 > x ||
             rect->x+rect->width     < x ||
             rect->y                 > y ||
             rect->y + rect->height  < y );
}