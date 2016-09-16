//
//  sprite_render.c
//  yuusha
//
//  Created by jukka on 16/8/25.
//  Copyright © 2016年 levelmax. All rights reserved.
//

#include "sprite_render.h"
#include "camera.h"

EXTERN_GAME;

#define VERTEX_PER_OBJECT (4)
void sprite_render_batch_reset(struct render_batch* self)
{
    self->offset = 0;
    self->tex_id = 0;
    self->n_objects = 0;
}

void sprite_render_func_flush(struct render* R)
{
    struct sprite_render_context* context = render_get_context(R, RENDER_TYPE_SPRITE);

    glBindVertexArray(context->buffer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, context->buffer->vbo);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_SIZE*context->n_objects * 4, context->buffer->data, GL_DYNAMIC_DRAW);

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
    unsigned long offset = 0;
    for (int i = 0; i < n; ++i) {
        struct render_batch* b = context->batches + i;
        glBindTexture(GL_TEXTURE_2D, b->tex_id);
        glDrawElements(GL_TRIANGLES, (b->n_objects)*6, GL_UNSIGNED_SHORT, (void*)offset);//);
        offset += b->n_objects * 6 * 2;
        sprite_render_batch_reset(b);
        R->drawcall++;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    context->current_batch_index = 0;
    context->n_objects = 0;
    context->buffer->offset = 0;
    context->current_batch = &context->batches[0];
    CHECK_GL_ERROR;
}

void sprite_render_func_start(struct render* R)
{
    GLuint prog = shader_get_program(R->shader, SHADER_COLOR);
    glUseProgram(prog);
    struct sprite_render_context* context = render_get_context(R, RENDER_TYPE_SPRITE);
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

/*-------------------- sprite_render --------------------*/
void sprite_render_func_draw(struct render* R, void* object)
{
    struct sprite* sprite = (struct sprite*)object;

    struct sprite_render_context* context = render_get_context(R, RENDER_TYPE_SPRITE);
    if (context->n_objects+1 > MAX_OBJECTS) {
        sprite_render_func_flush(R);
    }

    struct vertex_buffer* buffer = context->buffer;
    int offset = buffer->offset;
    struct vertex* data = buffer->data + offset;
    struct glyph* glyph = &sprite->glyph;
    data[0] = glyph->tr;
    data[1] = glyph->tl;
    data[2] = glyph->bl;
    data[3] = glyph->br;

    int new_tex_id = sprite->frame->tex_id;

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
    context->state.tex_id = sprite->frame->tex_id;
    context->n_objects++;

    buffer->offset += 4;
}

void sprite_render_func_destroy(struct render* R)
{

}

void sprite_render_func_end(struct render* R)
{
    // dummy
}

void sprite_render_func_init(struct render* R)
{
    struct sprite_render_context* context = STRUCT_NEW(sprite_render_context);
    struct vertex_buffer* buffer = STRUCT_NEW(vertex_buffer);

    // init the vao
    glGenVertexArrays(1, &buffer->vao);
    glBindVertexArray(buffer->vao);

    // init the vbo
    glGenBuffers(1, &buffer->vbo);
    glGenBuffers(1, &buffer->vibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->vibo);

    GLushort* idata = s_malloc(MAX_OBJECTS * 6 * sizeof(GLushort));
    //   tl(1) ------ (0) tr
    //      |         |
    //      |         |
    //      |         |
    //      |         |
    //   bl(2) -------(3) br
    for (int i = 0, j = 0; i < MAX_OBJECTS*6; i+=6, j+=4) {
        idata[i+0] = j+1;
        idata[i+1] = j+0;
        idata[i+2] = j+3;
        idata[i+3] = j+1;
        idata[i+4] = j+3;
        idata[i+5] = j+2;
    }

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*MAX_OBJECTS*6, idata, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    buffer->idata = idata;
    buffer->data = s_malloc(VERTEX_SIZE * MAX_OBJECTS * VERTEX_PER_OBJECT);
    buffer->offset = 0;

    context->state.tex_id = 0;
    context->buffer = buffer;
    memset(context->batches, 0, MAX_RENDER_BATCH*sizeof(struct render_batch));
    context->n_objects = 0;
    context->current_batch_index = 0;
    context->current_batch = &context->batches[0];

    struct render_func func = {
        sprite_render_func_init,
        sprite_render_func_start,
        sprite_render_func_end,
        sprite_render_func_flush,
        NULL,
    };

    struct render_object object = {
        RENDER_TYPE_SPRITE,
        func,
        context
    };
    render_set_object(R, &object);

    CHECK_GL_ERROR;
}