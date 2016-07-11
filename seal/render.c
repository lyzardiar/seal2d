#include "seal.h"

#include "geo.h"
#include "memory.h"
#include "shader.h"
#include "texture.h"

#include "render.h"
#include "camera.h"

EXTERN_GAME;

#define VERTEXES_PER_OBJECT (6)

struct vertex_buffer* vertex_buffer_new() {
    struct vertex_buffer* buffer = STRUCT_NEW(vertex_buffer);
    
    glGenVertexArrays(1, &buffer->vao);
    glBindVertexArray(buffer->vao);
    
    glGenBuffers(1, &buffer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, VERTEX_OFFSET_POS);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, VERTEX_SIZE, VERTEX_OFFSET_COLOR);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, VERTEX_OFFSET_UV);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    buffer->data = s_malloc(VERTEX_SIZE * MAX_OBJECTS);
    buffer->n_objs = 0;
    
    return buffer;
}

void vertex_buffer_free(struct vertex_buffer* self) {
    s_free(self->data);
    s_free(self);
}

static void vertex_buffer_add(struct vertex_buffer* self, const struct glyph* glyph) {
    int offset = (self->n_objs++) * VERTEXES_PER_OBJECT;
    struct vertex* buf = self->data + offset;
    buf[0] = glyph->tr;
    buf[1] = glyph->tl;
    buf[2] = glyph->bl;
    buf[3] = glyph->bl;
    buf[4] = glyph->br;
    buf[5] = glyph->tr;
}

struct render* render_new() {
    struct render* r = STRUCT_NEW(render);
    r->cur_program = 0;
    r->cur_tex_id = 0;
    r->vertex_buffer = vertex_buffer_new();
    r->render_state = 0;
    r->shader = shader_new();
    r->scissors.x = 0;
    r->scissors.y = 0;
    r->scissors.width = GAME->config.window_width;
    r->scissors.width = GAME->config.window_height;
    r->drawcall = 0;
    return r;
}

void render_free(struct render* self) {
    vertex_buffer_free(self->vertex_buffer);
    shader_free(self->shader);
    s_free(self);
}

void render_clear(struct render* self, color c) {
    float r = ((c >> 24) & 0xff) / 255.0;
    float g = ((c >> 16) & 0xff) / 255.0;
    float b = ((c >> 8 ) & 0xff) / 255.0;
    float a = ((c      ) & 0xff) / 255.0;
    
    glClearDepth(1.0f);
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    self->drawcall = 0;
}

void render_commit(struct render* self) {
    if (self->render_state & RENDER_STATE_PROGRAM_BIT) {
        glUseProgram(self->cur_program);
        
        float c4f[4] = {1.0f, 0.0f, 1.0f, 1.0f};
//        color_vec4(self->color, c4f);
        render_set_unfiorm(self, "mix_color", UNIFORM_4F, c4f);
    }

    struct vertex_buffer* vbuf = self->vertex_buffer;
    if (self->render_state & RENDER_STATE_VERTEX_BIT) {
        glBindBuffer(GL_ARRAY_BUFFER, vbuf->vbo);
        glBufferData(GL_ARRAY_BUFFER, VERTEX_SIZE*vbuf->n_objs * 6, vbuf->data, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindVertexArray(vbuf->vao);
    }
    
    // TODO: ungly hacking here.
    if (self->render_state & RENDER_STATE_TEXTURE_BIT) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, self->cur_tex_id);
        GLint texture_location = glGetUniformLocation(self->cur_program, "sampler");
        glUniform1i(texture_location, 0);
    }
    
    if (self->render_state) {
        GLint projection = glGetUniformLocation(self->cur_program, "projection");
        glUniformMatrix4fv(projection, 1, GL_FALSE, GAME->global_camera->camer_mat->m);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbuf->vbo);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, VERTEX_OFFSET_POS);
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, VERTEX_SIZE, VERTEX_OFFSET_COLOR);
        
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, VERTEX_OFFSET_UV);
        
        glDrawArrays(GL_TRIANGLES, 0, vbuf->n_objs * 6);
        self->drawcall++;
    }

    render_clear_state(self);
}

void render_clear_state(struct render* self) {
    self->render_state = 0;
    self->vertex_buffer->n_objs = 0;
}

void render_buffer_append(struct render* self, const struct glyph* glyph) {
    struct vertex_buffer* buffer = self->vertex_buffer;
    if (buffer->n_objs >= MAX_OBJECTS) {
        render_commit(self);
    }
    vertex_buffer_add(buffer, glyph);
    self->render_state |= RENDER_STATE_VERTEX_BIT;
}

void render_set_scissors(struct render* self, struct rect* rect) {
    if (self->render_state) {
        render_commit(self);
    }
    glEnable(GL_SCISSOR_TEST);
    glScissor(rect->x, rect->y, rect->width, rect->height);
}

void render_clear_scissors(struct render* self) {
    if (self->render_state) {
        render_commit(self);
    }
    glScissor(0, 0, GAME->config.window_width, GAME->config.window_height);
    glDisable(GL_SCISSOR_TEST);
    
    self->scissors.x = 0;
    self->scissors.y = 0;
    self->scissors.y = GAME->config.window_width;
    self->scissors.y = GAME->config.window_height;
}

void render_use_texture(struct render* self, GLuint tex_id) {
    self->cur_tex_id = tex_id;
    self->render_state |= RENDER_STATE_TEXTURE_BIT;
}

void render_use_shader(struct render* self, enum SHADER_TYPE shader_type) {
    GLuint program = shader_get_program(self->shader, shader_type);
    render_use_program(self, program);
}

void render_use_program(struct render* self, GLuint program) {
    self->cur_program = program;
    self->render_state |= RENDER_STATE_PROGRAM_BIT;
}

void render_set_unfiorm(struct render* self, const char* name, enum UNIFORM_TYPE uniform_type, float* v) {
    s_assert(self->cur_program);
    GLint location = shader_get_uniform(self->shader, self->cur_program, name);
    shader_set_uniform(self->shader, location, uniform_type, v);
}