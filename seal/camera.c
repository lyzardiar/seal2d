#include "camera.h"

struct camera* camera_new(float width, float height) {
    struct camera* c = (struct camera*)s_malloc(sizeof(struct camera));
    c->x = 0.0f;
    c->y = 0.0f;
    c->width = width;
    c->height = height;
    c->dirty = 0;
    c->mat = 0;
}

void camera_update(struct camera* c) {
    if(!c->dirty) {
        return;
    }
    c->mat
}