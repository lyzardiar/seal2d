#include "camera.h"

EXTERN_GAME;

struct camera* camera_new(float width, float height) {
    struct camera* c = (struct camera*)s_malloc(sizeof(struct camera));
    c->x = 0.0f;
    c->y = 0.0f;
    c->width = width;
    c->height = height;
    c->dirty = 0;
    c->orth_mat = orth_matrix(0, 0, GAME->window_width, GAME->window_height, -1.0f, 1.0f);
    c->camer_mat = identy_matix();
    return c;
}

void camera_free(struct camera* c) {
    matrix_free(c->orth_mat);
    matrix_free(c->camer_mat);
    s_free(c);
}

void camera_pos(struct camera* self, float x, float y) {
    self->x = x;
    self->y = y;
    
    self->dirty = 1;
}

void camera_update(struct camera* self) {
    if(!self->dirty) {
        return;
    }
    
    matrix_translate(self->orth_mat, self->camer_mat, -self->x, -self->y, 1.0);
}