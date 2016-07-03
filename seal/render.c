#include "seal.h"

#include "memory.h"
#include "shader.h"
#include "texture.h"

#include "render.h"
#include "camera.h"

EXTERN_GAME;

#define VERTEXES_PER_OBJECT (6)

struct vertex_buffer* vertex_buffer_new() {
    struct vertex_buffer* buffer = STRUCT_NEW(vertex_buffer);
    
    glGenBuffers(1, &buffer->vbo);
    CHECK_GL_ERROR;
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    CHECK_GL_ERROR;
    
    glGenVertexArrays(1, &buffer->vao);
    CHECK_GL_ERROR;
    glBindVertexArray(buffer->vao);
    CHECK_GL_ERROR;
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    CHECK_GL_ERROR;
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, VERTEX_OFFSET_POS);
    glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, VERTEX_SIZE, VERTEX_OFFSET_COLOR);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, VERTEX_OFFSET_UV);
    CHECK_GL_ERROR;
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    CHECK_GL_ERROR;
    
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

void vertex_buffer_add(struct vertex_buffer* self, struct glyph* glyph) {
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
    return r;
}

void render_free(struct render* self) {
    vertex_buffer_free(self->vertex_buffer);
    shader_free(self->shader);
    s_free(self);
}

void render_clear(struct render* self, unsigned long color) {
    render_commit(self);
    
    float r = ((color >> 24) & 0xff) / 255.0;
    float g = ((color >> 16) & 0xff) / 255.0;
    float b = ((color >> 8 ) & 0xff) / 255.0;
    float a = ((color      ) & 0xff) / 255.0;
    
    
//    glClearDepth(1.0f);
//    CHECK_GL_ERROR;
    glClearColor(r, g, b, a);
    CHECK_GL_ERROR;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    CHECK_GL_ERROR;
    
    glEnable(GL_BLEND);
    CHECK_GL_ERROR;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    CHECK_GL_ERROR;
}

void render_commit(struct render* self) {
    if (self->render_state & RENDER_STATE_PROGRAM_BIT) {
        glUseProgram(self->cur_program);
        CHECK_GL_ERROR;
    }

    struct vertex_buffer* vbuf = self->vertex_buffer;
    if (self->render_state & RENDER_STATE_VERTEX_BIT) {
        glBindBuffer(GL_ARRAY_BUFFER, vbuf->vbo);
        CHECK_GL_ERROR;
        glBufferData(GL_ARRAY_BUFFER, VERTEX_SIZE*vbuf->n_objs, vbuf->data, GL_DYNAMIC_DRAW);
        CHECK_GL_ERROR;
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERROR;
        
        glBindVertexArray(vbuf->vao);
        CHECK_GL_ERROR;
    }
    
    // TODO: ungly hacking here.
    if (self->render_state & RENDER_STATE_TEXTURE_BIT) {
        glActiveTexture(GL_TEXTURE0);
        CHECK_GL_ERROR;
        glBindTexture(GL_TEXTURE_2D, self->cur_tex_id);
        CHECK_GL_ERROR;
        GLint texture_location = glGetUniformLocation(self->cur_program, "sampler");
        CHECK_GL_ERROR;
        glUniform1i(texture_location, 0);
        CHECK_GL_ERROR;
    }
    
    if (self->render_state & RENDER_STATE_SCISSORS_BIT) {
        glEnable(GL_SCISSOR_TEST);
        CHECK_GL_ERROR;
        struct rect* scissors = &self->scissors;
        glScissor(scissors->x, scissors->y, scissors->width, scissors->height);
        CHECK_GL_ERROR;
    }
    
    if (self->render_state) {
        GLint projection = glGetUniformLocation(self->cur_program, "projection");
        CHECK_GL_ERROR;
        glUniformMatrix4fv(projection, 1, GL_FALSE, GAME->global_camera->camer_mat->m);
        CHECK_GL_ERROR;
        glDrawArrays(GL_TRIANGLES, 0, vbuf->n_objs);
        CHECK_GL_ERROR;
    }
    
    if (self->render_state & RENDER_STATE_VERTEX_BIT) {
        glBindVertexArray(0);
        CHECK_GL_ERROR;
    }
    
    render_clear_state(self);
    vbuf->n_objs = 0;
}

void render_clear_state(struct render* self) {
    glDisable(GL_SCISSOR_TEST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
    self->render_state = 0;
}

void render_buffer_append(struct render* self, struct glyph* glyph) {
    struct vertex_buffer* buffer = self->vertex_buffer;
    if (buffer->n_objs >= MAX_OBJECTS) {
        render_commit(self);
    }
    vertex_buffer_add(buffer, glyph);
    self->render_state |= RENDER_STATE_VERTEX_BIT;
}

void render_set_scissors(struct render* self, struct rect* rect) {
    self->scissors = *rect;
    self->render_state |= RENDER_STATE_SCISSORS_BIT;
}

void render_clear_scissors(struct render* self) {
    self->scissors.x = 0;
    self->scissors.y = 0;
    self->scissors.y = GAME->config.window_width;
    self->scissors.y = GAME->config.window_height;
}

void render_use_texture(struct render* self, GLuint tex_id) {
    if(tex_id != self->cur_tex_id) {
        self->cur_tex_id = tex_id;
        self->render_state |= RENDER_STATE_TEXTURE_BIT;
    }
}

void render_use_shader(struct render* self, enum SHADER_TYPE shader_type) {
    GLuint program = shader_get_program(self->shader, shader_type);
    render_use_program(self, program);
}

void render_use_program(struct render* self, GLuint program) {
    if (program != self->cur_program) {
        self->cur_program = program;
        self->render_state |= RENDER_STATE_PROGRAM_BIT;
    }
}