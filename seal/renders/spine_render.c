#include "spine_render.h"
#include "camera.h"

EXTERN_GAME;

#define VERTEX_PER_OBJECT (6)
void spine_render_func_init(struct render* R)
{
    struct spine_render_context* context = STRUCT_NEW(spine_render_context);
    struct vertex_buffer* buffer = STRUCT_NEW(vertex_buffer);

    // init the vao
    glGenVertexArrays(1, &buffer->vao);
    glBindVertexArray(buffer->vao);

    glGenBuffers(1, &buffer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vao);

    buffer->data = s_malloc(VERTEX_SIZE * MAX_OBJECTS * VERTEX_PER_OBJECT);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_SIZE * MAX_OBJECTS * VERTEX_PER_OBJECT, buffer->data, GL_DYNAMIC_DRAW);
    buffer->offset = 0;

    context->state.tex_id = 0;
    context->buffer = buffer;
    memset(context->batches, 0, MAX_RENDER_BATCH*sizeof(struct render_batch));
    context->n_objects = 0;
    context->current_batch_index = 0;

    struct render_func func = {
        spine_render_func_init,
        spine_render_func_start,
        spine_render_func_end,
        spine_render_func_flush,
        spine_render_destroy,
    };
    struct render_object object = {
        RENDER_TYPE_SPINE,
        func,
        context
    };
    render_set_object(R, &object);

    CHECK_GL_ERROR;
}

void spine_render_destroy(struct render* R)
{
    struct spine_render_context* context = render_get_context(R, RENDER_TYPE_SPINE);

    struct vertex_buffer* buffer = context->buffer;
    glDeleteBuffers(1, &buffer->vbo);
    glDeleteVertexArrays(1, &buffer->vao);
    s_free(context->buffer);

    s_free(context);
}

void spine_render_func_start(struct render* R)
{
    GLuint prog = shader_get_program(R->shader, SHADER_COLOR);
    glUseProgram(prog);
    struct spine_render_context* context = render_get_context(R, RENDER_TYPE_SPINE);
    context->state.program = prog;

    context->state.loc.position = glGetAttribLocation(context->state.program, "vertex_pos" );
    context->state.loc.color = glGetAttribLocation(context->state.program, "vertex_color" );
    context->state.loc.uv = glGetAttribLocation(context->state.program, "vertex_uv" );

    glActiveTexture(GL_TEXTURE0);
    GLint texture_location = glGetUniformLocation(prog, "texture_0");
    glUniform1i(texture_location, 0);

    render_set_mvp(prog, GAME->global_camera->camer_mat->m);
    CHECK_GL_ERROR;
}

void spine_render_func_draw(struct render* R, void* object)
{
    struct spine_render_vertex_desc* d = (struct spine_render_vertex_desc*)object;
    struct spine_render_context* context = render_get_context(R, RENDER_TYPE_SPINE);
    if (context->n_objects+1 > MAX_OBJECTS) {
        spine_render_func_flush(R);
    }

    // coord of spine
    // (x2, y2)------(x1, y1)
    //  |               |
    //  |               |
    //  |               |
    // (x3, y3)------(x4, y4)

    // copy the vertexes of the spine region attachment.
    float* v = d->vertex;
    float* uv = d->uv;

    int offset = context->buffer->offset;
    struct vertex* data = context->buffer->data + offset;
    SET_VERTEX_POS(data[0], v[0], v[1]);
    SET_VERTEX_POS(data[1], v[2], v[3]);
    SET_VERTEX_POS(data[2], v[6], v[7]);
    SET_VERTEX_POS(data[3], v[4], v[5]);
    SET_VERTEX_POS(data[4], v[6], v[7]);
    SET_VERTEX_POS(data[5], v[2], v[3]);

    SET_VERTEX_UV(data[0], uv[0], uv[1]);
    SET_VERTEX_UV(data[1], uv[2], uv[3]);
    SET_VERTEX_UV(data[2], uv[6], uv[7]);
    SET_VERTEX_UV(data[3], uv[4], uv[5]);
    SET_VERTEX_UV(data[4], uv[6], uv[7]);
    SET_VERTEX_UV(data[5], uv[2], uv[3]);

    for (int i = 0; i < 6; ++i) {
        SET_VERTEX_COLOR(data[i], 255, 255, 255, 255);
    }

    int n = context->current_batch_index;

    struct render_batch* batch = context->batches + n;

    GLuint new_tex_id = d->tex_id;

    if (new_tex_id == batch->tex_id) {
        batch->offset += VERTEX_PER_OBJECT;
        batch->n_objects++;
    } else {
        batch->offset = offset;
        batch->n_objects = 1;
        batch->tex_id = new_tex_id;

        context->state.tex_id = new_tex_id;
        context->current_batch_index++;
    }

    context->n_objects++;
}

void spine_render_func_end(struct render* R)
{

}

void spine_render_func_flush(struct render* R)
{
    struct spine_render_context* context = render_get_context(R, RENDER_TYPE_SPINE);

    glBindVertexArray(context->buffer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, context->buffer->vbo);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_SIZE*context->n_objects * 6, context->buffer->data, GL_DYNAMIC_DRAW);

    GLuint loc_pos = context->state.loc.position;
    GLuint loc_color = context->state.loc.color;
    GLuint loc_uv = context->state.loc.uv;
    glEnableVertexAttribArray(loc_pos);
    glEnableVertexAttribArray(loc_color);
    glEnableVertexAttribArray(loc_uv);
    glVertexAttribPointer(loc_pos, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, VERTEX_OFFSET_POS);
    glVertexAttribPointer(loc_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, VERTEX_SIZE, VERTEX_OFFSET_COLOR);
    glVertexAttribPointer(loc_uv, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, VERTEX_OFFSET_UV);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->buffer->vibo);
    int n = context->current_batch_index;
    for (int i = 0; i < n; ++i) {
        struct render_batch* b = context->batches + i;
        glBindTexture(GL_TEXTURE_2D, b->tex_id);
        glDrawArrays(GL_TRIANGLES, b->offset, b->n_objects*6);

        spine_render_batch_reset(b);
        R->drawcall++;
    }

    context->current_batch_index = 0;
    context->n_objects = 0;
    context->buffer->offset = 0;

    CHECK_GL_ERROR;
}

void spine_render_batch_reset(struct render_batch* self)
{
    self->offset = 0;
    self->tex_id = 0;
    self->n_objects = 0;
}

