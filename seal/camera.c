#include "seal.h"
#include "math/matrix.h"

#include "camera.h"

EXTERN_GAME;

struct camera* camera_new(float width, float height) {
    struct camera* c = (struct camera*)s_malloc(sizeof(struct camera));
    c->x = 0.0f;
    c->y = 0.0f;
    c->scale_x = 1.0f;
    c->scale_y = 1.0f;
    c->width = width;
    c->height = height;
    c->dirty = 0;

    c->camer_mat = orth_matrix(-GAME->window_width/2,
                               -GAME->window_height/2,
                               GAME->window_width/2,
                               GAME->window_height/2,
                               -1.0f, 1.0f);
    return c;
}

void camera_free(struct camera* c) {
    matrix_free(c->camer_mat);
    s_free(c);
}

void camera_pos(struct camera* self, float x, float y) {
    self->x = x;
    self->y = y;
    
    self->dirty |= TRANSLATE_DIRTY;
}

void camera_scale(struct camera* self, float sx, float sy) {
    self->scale_x = 1.0f;
    self->scale_y = 1.0f;
    
    self->dirty |= SCALE_DIRTY;
}

void camera_update(struct camera* self) {
    if(!self->dirty) {
        return;
    }
    
    matrix_translate(self->camer_mat, -self->x/GAME->window_width*2, -self->y/GAME->window_height*2, 1.0);
    self->dirty = 0;
    matrix_debug_print(self->camer_mat);
}