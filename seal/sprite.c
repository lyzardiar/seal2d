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

#define SET_VERTEX_UV(vert, u, v) \
    do { \
        vert.uv[0] = u; \
        vert.uv[1] = v; \
    } while(0);

sprite* sprite_alloc(float x, float y, float width, float height) {
    sprite
    * spr = (sprite*)s_malloc(sizeof(sprite));
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


    printf("sizeof(spr->vert) = %ld\n", sizeof(spr->vert));
    printf("sizeof(vertex) = %ld\n", sizeof(vertex));
    printf("offsetof(vertex, (vertex, position)) = %ld\n", offsetof(vertex, position));
    printf("offsetof(vertex, color) = %ld\n", offsetof(vertex, color));
    printf("offsetof(vertex, uv) = %ld\n", offsetof(vertex, uv));

    spr->texture = load_from_png("res/atlas_example.png");
    
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
    float dx = spr->speed_x * dt * 0.5;
    float dy = spr->speed_y * dt * 0.5;
    if (spr->x + dx >= 1.0f || spr->x + dx + spr->width <= -1.0f) {
        spr->speed_x = -spr->speed_x;
        spr->x -= dx;
    }
    
    if (spr->y + dy >= 1.0f || spr->y + dy  + spr->height <= -1.0f) {
        spr->speed_y = -spr->speed_y;
        spr->y -= dy;
    }
    
    sprite_set_pos(spr, spr->x + dx, spr->y + dy);
}

void sprite_draw(sprite* spr) {
    GLuint program = get_program(COLOR_SHADER);
    glUseProgram(program);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, spr->texture->id);
    GLint texture_location = glGetUniformLocation(program, "sampler");
    glUniform1i(texture_location, 0);
    
    
    glBindBuffer(GL_ARRAY_BUFFER, spr->vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vertex), (void*)offsetof(vertex, color));
    
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)offsetof(vertex, uv));
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glUseProgram(0);
}