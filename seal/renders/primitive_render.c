#include "primitive_render.h"
#include "camera.h"

EXTERN_GAME;

#define VERTICE_SIZE (1024*8)

void primitive_render_func_start(struct render* R) {
    GLuint prog = shader_get_program(R->shader, SHADER_COLOR);
    glUseProgram(prog);
    struct primitive_render_context* context = render_get_context(R, PRIMITIVE_RENDER);
    context->state.program = prog;

    context->state.loc.position = glGetAttribLocation(context->state.program, "vertex_pos" );
    context->state.loc.color = glGetAttribLocation(context->state.program, "vertex_color" );
    
    context->state.loc.uv = INVALID_ATTR_LOCATION; // there is no uv in primitive render.

    render_set_mvp(prog, GAME->global_camera->camer_mat->m);
    CHECK_GL_ERROR;
}


static void primitive_render_batch_reset(struct primitive_render_batch* self) {
    self->offset = 0;
    self->type = PRIMITIVE_INVALID;
    self->line_width = -1;
}


void primitive_render_func_flush(struct render* R) {
    struct primitive_render_context* context = render_get_context(R, PRIMITIVE_RENDER);
    glBindVertexArray(context->vao);

    glBindBuffer(GL_ARRAY_BUFFER, context->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * context->offset, context->vertexes, GL_DYNAMIC_DRAW);

    GLuint loc_pos = context->state.loc.position;
    GLuint loc_color = context->state.loc.color;

    glEnableVertexAttribArray(loc_pos);
    glEnableVertexAttribArray(loc_color);
    glVertexAttribPointer(loc_pos, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, VERTEX_OFFSET_POS);
    glVertexAttribPointer(loc_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, VERTEX_SIZE, VERTEX_OFFSET_COLOR);

    int n = context->current_batch_index;

    for (int i = 0; i < n; ++i) {
        struct primitive_render_batch* b = context->batches + i;
        switch (b->type) {
            case PRIMITIVE_LINE:
            {
                glLineWidth(b->line_width);
                glDrawArrays(GL_LINE, 0, b->line_count);
            }
                break;
                
            default:
                break;
        }
    }

    context->current_batch_index = 0;
    context->offset = 0;
}

void primitive_render_func_draw(struct render* R, enum primitive_type primitive_type, void* object) {
    static int vertex_size[PRIMITVE_MAX] = {
        4, // line takes 4 floats
        8  // rect takes 8 floats
    };

    struct primitive_render_context* context = render_get_context(R, PRIMITIVE_RENDER);
    if (context->current_batch_index >= MAX_RENDER_BATCH ||
        context->offset + vertex_size[primitive_type] >= VERTICE_SIZE) {
        primitive_render_func_flush(R);
    }

    float* buffer = context->vertexes;
    int offset = context->offset;
    float* data = buffer + context->offset;

    struct sprite* sprite = (struct sprite*) object;
    struct primitive_render_batch* batch = context->batches + context->current_batch_index;

    switch (primitive_type) {
        case PRIMITIVE_LINE:
            {
                float* v = sprite->primitive_vertex;
                data[0] = v[0];
                data[1] = v[1];
                data[2] = v[2];
                data[3] = v[3];

                if (context->state.last_type != PRIMITIVE_LINE ||
                    context->state.line_width != sprite->line_width) {
                    batch->line_width = sprite->line_width;
                    batch->offset = offset;
                    batch->type = PRIMITIVE_LINE;

                    context->state.line_width = sprite->line_width;
                    context->current_batch_index++;

                } else {
                    batch->offset += 2;
                }
                context->offset += 2;
            }
            break;

        case PRIMITIVE_RECT:

            break;

        default:
            break;
    }

}

void primitive_render_func_end(struct render* R) {

}


void primitive_render_func_init(struct render* R) {
    R->R_objs[SPRITE_RENDER].type = SPRITE_RENDER;
    R->R_objs[SPRITE_RENDER].render_func.init = primitive_render_func_init;
    R->R_objs[SPRITE_RENDER].render_func.start = primitive_render_func_start;
    R->R_objs[SPRITE_RENDER].render_func.end = primitive_render_func_end;
    R->R_objs[SPRITE_RENDER].render_func.flush = primitive_render_func_flush;

    struct primitive_render_context* context = STRUCT_NEW(primitive_render_context);

    glGenBuffers(1, &context->vbo);

    glGenVertexArrays(1, &context->vao);
    glBindVertexArray(context->vao);

    context->state.last_type = PRIMITIVE_INVALID;
    context->state.line_width = -1;
    context->state.program = -1;
    
    context->current_batch_index = 0;
    context->n_objects = 0;

    memset(context->batches, 0, MAX_RENDER_BATCH*sizeof(struct primitive_render_batch));

    render_set_context(R, PRIMITIVE_RENDER, context);
    
    CHECK_GL_ERROR;
}