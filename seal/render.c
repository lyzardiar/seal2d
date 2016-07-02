#include "memory.h"

#include "render.h"

struct vertex_buffer* vertex_buffer_new() {
    struct vertex_buffer* buffer = STRUCT_NEW(vertex_buffer);
    glGenBuffers(1, &buffer->vbo);
    glGenVertexArrays(1, &buffer->vao);
    
    buffer->data = s_malloc(VERTEX_SIZE * MAX_VERTEX);
    buffer->stride = 0;
    buffer->n = 0;
    
    return buffer;
}

void vertex_buffer_free(struct vertex_buffer* self) {
    s_free(self->data);
    s_free(self);
}


struct render* render_new() {
    struct render* r = STRUCT_NEW(render);
    r->cur_program = 0;
    r->cur_texture = NULL;
    r->vertex_buffer = vertex_buffer_new();
    return r;
}

void render_free(struct render* self) {
    vertex_buffer_free(self->vertex_buffer);
    s_free(self);
}

void render_clear(struct render* self, unsigned long color) {
    render_commit(self);
    
    float r = ((color >> 24) & 0xff) / 255.0;
    float g = ((color >> 16) & 0xff) / 255.0;
    float b = ((color >> 8 ) & 0xff) / 255.0;
    float a = ((color      ) & 0xff) / 255.0;
    
//    glClearDepth(1.0f);
    glClearColor(r, g, b, a);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void render_commit(struct render* self) {
    
}

void render_buffer_append(struct render* self, struct glyph* glyph) {
    struct vertex_buffer* buffer = self->vertex_buffer;
    if (buffer->n >= MAX_VERTEX) {
        
    }
}

void render_set_scissors(struct render* self, struct rect* rect);
void render_clear_scissors(struct render* self);

void render_use_texture(struct render* self, struct texture* texture);
void render_use_program(GLuint program);