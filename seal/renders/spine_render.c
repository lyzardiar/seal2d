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
    buffer->offset = 0;

    context->state.tex_id = 0;
    context->buffer = buffer;
    memset(context->batches, 0, MAX_RENDER_BATCH*sizeof(struct render_batch));
    context->n_objects = 0;
    context->current_batch_index = 0;
    context->current_batch = &context->batches[0];

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
    // v1(x2, y2)------v0(x1, y1)
    //  |               |
    //  |               |
    //  |               |
    // v2(x3, y3)------v3(x4, y4)

    // copy the vertexes of the spine region attachment.
    float* v = d->vertex;
    float* uv = d->uv;

    int offset = context->buffer->offset;
    struct vertex* data = context->buffer->data + offset;
    SET_VERTEX_POS(data[0], v[0] + 250.0f, v[1]);
    SET_VERTEX_POS(data[1], v[2] + 250.0f, v[3]);
    SET_VERTEX_POS(data[2], v[6] + 250.0f, v[7]);
    SET_VERTEX_POS(data[3], v[4] + 250.0f, v[5]);
    SET_VERTEX_POS(data[4], v[6] + 250.0f, v[7]);
    SET_VERTEX_POS(data[5], v[2] + 250.0f, v[3]);

    SET_VERTEX_UV(data[0], uv[0], 1.0 - uv[1]);
    SET_VERTEX_UV(data[1], uv[2], 1.0 - uv[3]);
    SET_VERTEX_UV(data[2], uv[6], 1.0 - uv[7]);
    SET_VERTEX_UV(data[3], uv[4], 1.0 - uv[5]);
    SET_VERTEX_UV(data[4], uv[6], 1.0 - uv[7]);
    SET_VERTEX_UV(data[5], uv[2], 1.0 - uv[3]);

    for (int i = 0; i < 6; ++i) {
        SET_VERTEX_COLOR(data[i], 255, 255, 255, 255);
    }

    struct vertex_buffer* buffer = context->buffer;

    GLuint new_tex_id = d->tex_id;

    struct render_batch* cur_batch = context->current_batch;
    if (new_tex_id == cur_batch->tex_id) {
        cur_batch->n_objects++;
    } else {
        struct render_batch* new_batch = &context->batches[context->current_batch_index];
        new_batch->offset = buffer->offset;
        new_batch->n_objects = 1;
        new_batch->tex_id = new_tex_id;
        context->current_batch = new_batch;
        context->current_batch_index++;
    }

    // save the last tex_id
    context->state.tex_id = new_tex_id;
    context->n_objects++;

    buffer->offset += 6;
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
        CHECK_GL_ERROR;
        glDrawArrays(GL_TRIANGLES, b->offset, b->n_objects*6);
        CHECK_GL_ERROR;
        spine_render_batch_reset(b);
        R->drawcall++;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    context->current_batch_index = 0;
    context->n_objects = 0;
    context->buffer->offset = 0;
    context->current_batch = &context->batches[0];

    CHECK_GL_ERROR;
}

void spine_render_batch_reset(struct render_batch* self)
{
    self->offset = 0;
    self->tex_id = 0;
    self->n_objects = 0;
}

