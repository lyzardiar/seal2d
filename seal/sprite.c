#include "sprite.h"

#define SET_VERTEX_POS(vert, x, y) \
    do { \
        vert.position[0] = x; \
        vert.position[1] = y; \
    } while(0);

#define SET_VERTEX_COLOR(vert, r, g, b, a) \
    do { \
        vert.color[0] = r; \
        vert.color[1] = g; \
        vert.color[2] = b; \
        vert.color[3] = a; \
    } while(0);

sprite* sprite_alloc(float x, float y, float width, float height) {
    sprite* spr = SDL_malloc(sizeof(sprite));
    spr->x = x;
    spr->y = y;
    spr->width = width;
    spr->height = height;
    spr->vbo = 0;
    
    //   A------D
    //   |      |
    //   |      |
    //   B------C
    //
    //   A(x, y+height)
    //   B(x, y)
    //   C(x+width, y)
    //   D(x+width, y+height)
    // we draw this in counter-clock wise.
    SET_VERTEX_POS(spr->vert[0], x+width, y+height);
    SET_VERTEX_POS(spr->vert[1], x, y+height);
    SET_VERTEX_POS(spr->vert[2], x, y);
    SET_VERTEX_POS(spr->vert[3], x, y);
    SET_VERTEX_POS(spr->vert[4], x+width, y);
    SET_VERTEX_POS(spr->vert[5], x+width, y+height);
    
    SET_VERTEX_COLOR(spr->vert[0], 255, 0, 0, 255);
    SET_VERTEX_COLOR(spr->vert[1], 255, 0, 0, 255);
    SET_VERTEX_COLOR(spr->vert[2], 255, 0, 0, 255);
    SET_VERTEX_COLOR(spr->vert[3], 255, 0, 0, 255);
    SET_VERTEX_COLOR(spr->vert[4], 255, 0, 0, 255);
    SET_VERTEX_COLOR(spr->vert[5], 255, 0, 0, 255);
    
    printf("sizeof(spr->vert) = %ld\n", sizeof(spr->vert));

    glGenBuffers(1, &spr->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, spr->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spr->vert), spr->vert, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    return spr;
}

void sprite_free(sprite* spr) {
    
    SDL_free(spr);
}

void sprite_set_texture(sprite* spr, const char* file_name) {
    
}

void sprite_set_pos(sprite* spr, float x, float y) {
    spr->x = x;
    spr->y = y;
}

void sprite_set_size(sprite* spr, float width, float height) {
    spr->width = width;
    spr->height = height;
}

void sprite_update(sprite* spr, float dt) {
    
}

void sprite_draw(sprite* spr) {
    
}