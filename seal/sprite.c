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
    sprite* spr = (sprite*)s_malloc(sizeof(sprite));
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
//    SET_VERTEX_POS(spr->vert[0], x+width, y+height);
//    SET_VERTEX_POS(spr->vert[1], x, y+height);
//    SET_VERTEX_POS(spr->vert[2], x, y);
//    SET_VERTEX_POS(spr->vert[3], x, y);
//    SET_VERTEX_POS(spr->vert[4], x+width, y);
//    SET_VERTEX_POS(spr->vert[5], x+width, y+height);
//    
//    SET_VERTEX_COLOR(spr->vert[0], 255, 0, 0, 255);
//    SET_VERTEX_COLOR(spr->vert[1], 255, 0, 0, 255);
//    SET_VERTEX_COLOR(spr->vert[2], 255, 0, 0, 255);
//    SET_VERTEX_COLOR(spr->vert[3], 255, 0, 0, 255);
//    SET_VERTEX_COLOR(spr->vert[4], 255, 0, 0, 255);
//    SET_VERTEX_COLOR(spr->vert[5], 255, 0, 0, 255);
//    

    GLfloat vertex[2*3] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
    };
    glGenBuffers(1, &spr->vbo);
    CHECK_GL_ERROR
    glBindBuffer(GL_ARRAY_BUFFER, spr->vbo);
    CHECK_GL_ERROR
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

//    glBufferData(GL_ARRAY_BUFFER, sizeof(spr->vert), spr->vert, GL_STATIC_DRAW);
    
    CHECK_GL_ERROR
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    CHECK_GL_ERROR
    glGenVertexArrays(1, &spr->vao);
    CHECK_GL_ERROR
    glBindVertexArray(spr->vao);
    CHECK_GL_ERROR
    
//    glEnable(GL_CULL_FACE);
//    CHECK_GL_ERROR
//    glCullFace(GL_BACK);
//    CHECK_GL_ERROR
//    glFrontFace(GL_CW);
//    CHECK_GL_ERROR
    
    printf("sizeof(spr->vert) = %ld\n", sizeof(spr->vert));
    printf("sizeof(vertex) = %ld\n", sizeof(vertex));
//    printf("(void*)offsetof(vertex, position) = %d\n", (void*)offsetof(vertex, position));
//    printf("(void*)offsetof(vertex, color) = %d\n", (void*)offsetof(vertex, color));
    return spr;
}

void sprite_free(sprite* spr) {
    s_free(spr);
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
    GLuint program = get_program(COLOR_SHADER);
    glUseProgram(program);
    CHECK_GL_ERROR;
    glBindBuffer(GL_ARRAY_BUFFER, spr->vbo);
    CHECK_GL_ERROR;
    glEnableVertexAttribArray(0);
    CHECK_GL_ERROR;

//    glEnableVertexAttribArray(1);
//    CHECK_GL_ERROR;

//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(spr->vert), (void*)offsetof(vertex, position));
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    CHECK_GL_ERROR;

//    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(spr->vert), (void*)offsetof(vertex, color));
//    CHECK_GL_ERROR;
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    CHECK_GL_ERROR;

    glDisableVertexAttribArray(0);
//    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glUseProgram(0);

}