#ifndef render_h
#define render_h

#include <OpenGL/gl3.h>
#include "geo.h"

#define RENDER_STATE_TEXTURE_BIT  (1)
#define RENDER_STATE_SCISSORS_BIT (1 >> 1)

struct glyph;
struct texture;
struct rect;
struct array;

struct vertex_buffer {
    GLuint vbo;
    GLuint vao;
    void* data;
    int stride;
    int n;
};

struct render {
    GLuint cur_program;
    struct texture* cur_texture;
    
    struct vertex_buffer* vertex_buffer;
    struct rect* scissors;
};

struct vertex_buffer* vertex_buffer_new();
void vertex_buffer_free(struct vertex_buffer* self);

struct render* render_new();
void render_free(struct render* self);

void render_clear(struct render* self, unsigned long color);
void render_commit(struct render* self);

void render_buffer_append(struct render* self, struct glyph* glyph);

void render_set_scissors(struct render* self, struct rect* rect);
void render_clear_scissors(struct render* self);

void render_use_texture(struct render* self, struct texture* texture);
void render_use_program(GLuint program);

#endif /* render_h */
