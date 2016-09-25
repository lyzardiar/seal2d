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

    context->buffer = buffer;

    render_context_reset(&context->__super);

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

    struct render_state* state = &context->__super.state;
    state->program = prog;

    state->loc.position = glGetAttribLocation(prog, "vertex_pos" );
    state->loc.color = glGetAttribLocation(prog, "vertex_color" );
    state->loc.uv = glGetAttribLocation(prog, "vertex_uv" );

    glActiveTexture(GL_TEXTURE0);
    GLint texture_location = glGetUniformLocation(prog, "texture_0");
    glUniform1i(texture_location, 0);

    render_set_mvp(prog, GAME->global_camera->camer_mat->m);
    CHECK_GL_ERROR;
}

static void spine_render_update_batch(struct render* R,
                                      struct spine_render_context* context,
                                      int tex_id,
                                      int offset)
{
    struct vertex_buffer* buffer = context->buffer;

    context->__super.n_objects++;
    buffer->offset += 6;

    struct render_batch* cur_batch = context->__super.current_batch;
    if (tex_id == cur_batch->tex_id) {
        cur_batch->n_objects++;
    } else {
        if(render_context_update_batch(&context->__super, offset, tex_id)) {
            spine_render_func_flush(R);
        }
    }
}

void spine_render_func_draw(struct render* R, void* object)
{
    struct spine_render_vertex_desc* spine_vertex = (struct spine_render_vertex_desc*)object;
    struct spine_render_context* context = render_get_context(R, RENDER_TYPE_SPINE);
    if (render_context_object_incfull(&context->__super)) {
        spine_render_func_flush(R);
    }

    /* coord of spine
     * v1(x2, y2)------v0(x1, y1)
     *  |               |
     *  |               |
     *  |               |
     * v2(x3, y3)------v3(x4, y4)
     */

    // copy the vertexes of the spine region attachment.
    float* v = spine_vertex->vertex;
    float* uv = spine_vertex->uv;
    float x = spine_vertex->x;
    float y = spine_vertex->y;
    int offset = context->buffer->offset;
    struct vertex* data = context->buffer->data + offset;
    SET_VERTEX_POS(data[0], v[0] + x, v[1] + y);
    SET_VERTEX_POS(data[1], v[2] + x, v[3] + y);
    SET_VERTEX_POS(data[2], v[6] + x, v[7] + y);
    SET_VERTEX_POS(data[3], v[4] + x, v[5] + y);
    SET_VERTEX_POS(data[4], v[6] + x, v[7] + y);
    SET_VERTEX_POS(data[5], v[2] + x, v[3] + y);

    SET_VERTEX_UV(data[0], uv[0], 1.0 - uv[1]);
    SET_VERTEX_UV(data[1], uv[2], 1.0 - uv[3]);
    SET_VERTEX_UV(data[2], uv[6], 1.0 - uv[7]);
    SET_VERTEX_UV(data[3], uv[4], 1.0 - uv[5]);
    SET_VERTEX_UV(data[4], uv[6], 1.0 - uv[7]);
    SET_VERTEX_UV(data[5], uv[2], 1.0 - uv[3]);

    for (int i = 0; i < 6; ++i) {
        SET_VERTEX_COLOR(data[i], 255, 255, 255, 255);
    }

    spine_render_update_batch(R, context, spine_vertex->tex_id, offset);
}

void spine_render_func_end(struct render* R)
{

}

void spine_render_func_flush(struct render* R)
{
    struct spine_render_context* ctx = render_get_context(R, RENDER_TYPE_SPINE);


    glBindVertexArray(ctx->buffer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, ctx->buffer->vbo);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_SIZE*ctx->__super.n_objects * 6,
                                               ctx->buffer->data,
                                               GL_DYNAMIC_DRAW);

    GLuint loc_pos = ctx->__super.state.loc.position;
    GLuint loc_color = ctx->__super.state.loc.color;
    GLuint loc_uv = ctx->__super.state.loc.uv;
    glEnableVertexAttribArray(loc_pos);
    glEnableVertexAttribArray(loc_color);
    glEnableVertexAttribArray(loc_uv);
    glVertexAttribPointer(loc_pos, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, VERTEX_OFFSET_POS);
    glVertexAttribPointer(loc_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, VERTEX_SIZE, VERTEX_OFFSET_COLOR);
    glVertexAttribPointer(loc_uv, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, VERTEX_OFFSET_UV);

    int n = ctx->__super.current_batch_index;
    for (int i = 0; i < n; ++i) {
        struct render_batch* b = ctx->__super.batches + i;
        glBindTexture(GL_TEXTURE_2D, b->tex_id);
        glDrawArrays(GL_TRIANGLES, b->offset, b->n_objects*6);
        spine_render_batch_reset(b);
        R->drawcall++;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    render_context_reset(&ctx->__super);
    ctx->buffer->offset = 0;
    CHECK_GL_ERROR;
}

void spine_render_batch_reset(struct render_batch* self)
{
    self->offset = 0;
    self->tex_id = 0;
    self->n_objects = 0;
}
