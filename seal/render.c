#include "seal.h"

#include "geo.h"
#include "memory.h"
#include "shader.h"
#include "texture.h"
#include "array.h"

#include "platform/render_opengl.h"
#include "render.h"
#include "shader.h"
#include "camera.h"

EXTERN_GAME;

#define VERTEX_PER_OBJECT (4)
static void sprite_render_batch_reset(struct render_batch* self) {
    self->offset = 0;
    self->tex_id = 0;
    self->n_objects = 0;
}

static void sprite_render_func_init(struct render* R) {
    struct sprite_render_context* context = STRUCT_NEW(sprite_render_context);
    struct vertex_buffer* buffer = STRUCT_NEW(vertex_buffer);
    
    // init the vao
    glGenVertexArrays(1, &buffer->vao);
    glBindVertexArray(buffer->vao);

    // init the vbo
    glGenBuffers(1, &buffer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);

    // init the vertex index buffer
    CHECK_GL_ERROR;
    glGenBuffers(1, &buffer->vibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->vibo);
    
    GLushort* idata = s_malloc(MAX_OBJECTS * 6 * sizeof(GLushort));
    int i, j;
    
    //   tl(1) ------ (0) tr
    //      |         |
    //      |         |
    //      |         |
    //      |         |
    //   bl(2) -------(3) br
    for (i = 0, j = 0; i < MAX_OBJECTS*6; i+=6, j+=4) {
        idata[i+0] = j+1;
        idata[i+1] = j+0;
        idata[i+2] = j+3;
        idata[i+3] = j+1;
        idata[i+4] = j+3;
        idata[i+5] = j+2;
    }
    
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*MAX_OBJECTS*6, idata, GL_STATIC_DRAW);
    CHECK_GL_ERROR;

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    buffer->idata = idata;
    
    buffer->data = s_malloc(VERTEX_SIZE * MAX_OBJECTS * VERTEX_PER_OBJECT);
    buffer->offset = 0;
    
    context->state.tex_id = 0;
    context->buffer = buffer;
    memset(context->batches, 0, MAX_RENDER_BATCH*sizeof(struct render_batch));
    context->n_objects = 0;
    context->current_batch_index = 0;
    
    render_set_context(R, context);
}

void sprite_render_func_flush(struct render* R) {
    struct sprite_render_context* context = (R->context);
    
    glBindVertexArray(context->buffer->vao);

    glBindBuffer(GL_ARRAY_BUFFER, context->buffer->vbo);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_SIZE*context->n_objects * 4, context->buffer->data, GL_DYNAMIC_DRAW);

////    glEnableVertexAttribArray(2);
//    printf("VERTEX_SIZE = %d, VERTEX_OFFSET_POS = %d, VERTEX_OFFSET_COLOR = %d,",
//           VERTEX_SIZE,
//           VERTEX_OFFSET_POS,
//           VERTEX_OFFSET_COLOR);


    GLuint loc_pos = context->state.loc.position;
    GLuint loc_color = context->state.loc.color;
    GLuint loc_uv = context->state.loc.uv;
    glEnableVertexAttribArray(loc_pos);
    glEnableVertexAttribArray(loc_color);
    glEnableVertexAttribArray(loc_uv);
    glVertexAttribPointer(loc_pos, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, VERTEX_OFFSET_POS);
    glVertexAttribPointer(loc_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, VERTEX_SIZE, VERTEX_OFFSET_COLOR);
    glVertexAttribPointer(loc_uv, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, VERTEX_OFFSET_UV);

//    char buff[1024] = "";
//    for (int i = 0; i < context->n_objects*4; ++i) {
//        vertex_tostring(buff, context->buffer->data + i);
//        printf("%s\n", buff);
//    }


    int n = context->current_batch_index;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, context->buffer->vibo);
    for (int i = 0; i < n; ++i) {
        struct render_batch* b = context->batches + i;
        glBindTexture(GL_TEXTURE_2D, b->tex_id);
        glDrawElements(GL_TRIANGLES, b->n_objects*6, GL_UNSIGNED_SHORT, 0);
        
        sprite_render_batch_reset(b);
        R->drawcall++;
    }
    
    context->current_batch_index = 0;
    context->n_objects = 0;
    context->buffer->offset = 0;
}

void sprite_render_func_start(struct render* R) {
    GLuint prog = shader_get_program(R->shader, SHADER_COLOR);
    glUseProgram(prog);
    struct sprite_render_context* context = R->context;
    context->state.program = prog;

    context->state.loc.position = glGetAttribLocation(context->state.program, "vertex_pos" );
    context->state.loc.color = glGetAttribLocation(context->state.program, "vertex_color" );
    context->state.loc.uv = glGetAttribLocation(context->state.program, "vertex_uv" );

    glActiveTexture(GL_TEXTURE0);
    GLint texture_location = glGetUniformLocation(prog, "texture_0");
    glUniform1i(texture_location, 0);

//    GLint projection = glGetUniformLocation(prog, "projection");
//    glUniformMatrix4fv(projection, 1, GL_FALSE, GAME->global_camera->camer_mat->m);
    CHECK_GL_ERROR;
}

/*-------------------- sprite_render --------------------*/
void sprite_render_func_draw(struct render* R, void* object) {
    struct sprite* sprite = (struct sprite*)object;
    
    struct sprite_render_context* context = (R->context);
    if (context->n_objects+1 > MAX_OBJECTS) {
        sprite_render_func_flush(R);
        return;
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
    
    int n = context->current_batch_index;
    
    struct render_batch* batch = context->batches + n;
    
    if (new_tex_id == batch->tex_id) {
        batch->offset += VERTEX_PER_OBJECT;
        batch->n_objects++;
    } else {
        batch->offset = offset;
        batch->n_objects = 1;
        batch->tex_id = new_tex_id;
        context->current_batch_index++;
    }
    
    // save the last tex_id
    context->state.tex_id = sprite->frame->tex_id;
    context->n_objects++;
    
    buffer->offset = offset;
}

void sprite_render_func_end(struct render* R) {
    // dummy
}

/*-------------------- render --------------------*/

#define RENDER_INVALID (-1)
struct render* render_new() {
    struct render* r = STRUCT_NEW(render);
    r->last = r->current = RENDER_INVALID;
    r->context = NULL;
    
    r->R_objs[SPRITE_RENDER].type = SPRITE_RENDER;
    r->R_objs[SPRITE_RENDER].render_func.init = sprite_render_func_init;
    r->R_objs[SPRITE_RENDER].render_func.start = sprite_render_func_start;
    r->R_objs[SPRITE_RENDER].render_func.draw = sprite_render_func_draw;
    r->R_objs[SPRITE_RENDER].render_func.end = sprite_render_func_end;
    r->R_objs[SPRITE_RENDER].render_func.flush = sprite_render_func_flush;
    
    // TODO: move to render switch to do lazy init.
    sprite_render_func_init(r);
    
    r->shader = shader_new();
    return r;
}

void render_set_context(struct render* self, void* context) {
    self->context = context;
}

void render_free(struct render* self) {
    shader_free(self->shader);
    s_free(self);
}

void render_clear(struct render* self, color c) {
    float r = ((c >> 24) & 0xff) / 255.0;
    float g = ((c >> 16) & 0xff) / 255.0;
    float b = ((c >> 8 ) & 0xff) / 255.0;
    float a = ((c      ) & 0xff) / 255.0;

    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    self->drawcall = 0;
}

void render_switch(struct render* self, enum RENDER_TYPE type) {
    if (self->current != type) {
        if (self->last != RENDER_INVALID) {
            struct render_object* LR = &self->R_objs[self->last];
            LR->render_func.flush(self);
        }
    }
    
    struct render_object* render_object = &self->R_objs[type];
    render_object->render_func.start(self);
    
    self->current = type;
    self->last = self->current;
}
