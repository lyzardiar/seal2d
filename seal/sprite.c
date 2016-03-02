#include "sprite.h"

EXTERN_GAME;

sprite* sprite_alloc(float x, float y, float width, float height) {
    sprite * spr = (sprite*)s_malloc(sizeof(sprite));
    spr->x = x;
    spr->y = y;
    spr->width = width;
    spr->height = height;
    spr->vbo = 0;
    spr->vao = 0;
    
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
    SET_VERTEX_POS(spr->vert[0], x+width, y+height);  // D
    SET_VERTEX_POS(spr->vert[1], x, y+height);        // A
    SET_VERTEX_POS(spr->vert[2], x, y);               // B
    SET_VERTEX_POS(spr->vert[3], x, y);               // B
    SET_VERTEX_POS(spr->vert[4], x+width, y);         // C
    SET_VERTEX_POS(spr->vert[5], x+width, y+height);  // D
    
    for (int i = 0; i < 6; ++i) {
        SET_VERTEX_COLOR(spr->vert[i], 255, 255, 255, 255);
    }
    
    SET_VERTEX_UV(spr->vert[0], 1.0f, 1.0f);
    SET_VERTEX_UV(spr->vert[1], 0.0f, 1.0f);
    SET_VERTEX_UV(spr->vert[2], 0.0f, 0.0f);
    SET_VERTEX_UV(spr->vert[3], 0.0f, 0.0f);
    SET_VERTEX_UV(spr->vert[4], 1.0f, 0.0f);
    SET_VERTEX_UV(spr->vert[5], 1.0f, 1.0f);
    printf("sizeof(sizeof(vertexes)) = %ld\n", sizeof(spr->vert));
    glGenBuffers(1, &spr->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, spr->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spr->vert), spr->vert, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glGenVertexArrays(1, &spr->vao);
    glBindVertexArray(spr->vao);

    spr->texture = texture_load_from_png("res/atlas_example.png");
    
    spr->speed_x = 1;
    spr->speed_y = 1; //5.0f;
    return spr;
}

void sprite_free(sprite* spr) {
    s_free(spr->texture);
    s_free(spr);
}

void sprite_set_texture(sprite* spr, const char* file_name) {
    
}

void sprite_set_pos(sprite* spr, float x, float y) {
    spr->x = x;
    spr->y = y;
    
    SET_VERTEX_POS(spr->vert[0], x+spr->width, y+spr->height);
    SET_VERTEX_POS(spr->vert[1], x, y+spr->height);
    SET_VERTEX_POS(spr->vert[2], x, y);
    SET_VERTEX_POS(spr->vert[3], x, y);
    SET_VERTEX_POS(spr->vert[4], x+spr->width, y);
    SET_VERTEX_POS(spr->vert[5], x+spr->width, y+spr->height);
    
    glBindBuffer(GL_ARRAY_BUFFER, spr->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(spr->vert), spr->vert, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void sprite_set_size(sprite* spr, float width, float height) {
    spr->width = width;
    spr->height = height;
}

void sprite_update(sprite* spr, float dt) {

}

void sprite_draw(sprite* spr) {

}