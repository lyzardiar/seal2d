#ifndef render_h
#define render_h

#include <OpenGL/gl3.h>
#include "geo.h"
#include "shader.h"

#define RENDER_STATE_TEXTURE_BIT  (1)
#define RENDER_STATE_VERTEX_BIT   (1 << 1)
#define RENDER_STATE_SCISSORS_BIT (1 << 2)
#define RENDER_STATE_PROGRAM_BIT  (1 << 3)

struct glyph;
struct array;

struct vertex_buffer {
    GLuint vbo;
    GLuint vao;
    struct vertex* data;
    int n_objs;
};

struct render {
    struct shader* shader;
    
    GLuint cur_tex_id;
    GLuint cur_program;
    
    struct vertex_buffer* vertex_buffer;
    struct rect scissors;
    
    unsigned int render_state;
    
    unsigned int drawcall;
};

struct vertex_buffer* vertex_buffer_new();
void vertex_buffer_free(struct vertex_buffer* self);

struct render* render_new();
void render_free(struct render* self);

void render_clear_state(struct render* self);
void render_clear(struct render* self, unsigned long color);
void render_commit(struct render* self);

void render_buffer_append(struct render* self, const struct glyph* glyph);

void render_set_scissors(struct render* self, struct rect* rect);
void render_clear_scissors(struct render* self);

void render_use_texture(struct render* self, GLuint tex_id);
void render_use_shader(struct render* self, enum SHADER_TYPE shader_type);
void render_use_program(struct render* self, GLuint program);

#endif /* render_h */
