#include "primitive_render.h"
#include "camera.h"

EXTERN_GAME;

#define VERTICE_SIZE (1024*8)

void primitive_render_func_start(struct render* R)
{
    GLuint prog = shader_get_program(R->shader, SHADER_PRIMITIVE);
    glUseProgram(prog);
    struct primitive_render_context* context = render_get_context(R, PRIMITIVE_RENDER);
    context->state.program = prog;

    context->state.loc.position = glGetAttribLocation(context->state.program, "vertex_pos" );
    context->state.loc.color = glGetAttribLocation(context->state.program, "vertex_color" );
    context->state.loc.uv = INVALID_ATTR_LOCATION; // there is no uv in primitive render.

    render_set_mvp(prog, GAME->global_camera->camer_mat->m);
    CHECK_GL_ERROR;
}


static void primitive_render_batch_reset(struct primitive_render_batch* self)
{
    self->offset = 0;
    self->type = PRIMITIVE_INVALID;
    self->line_width = -1;
}


void primitive_render_func_flush(struct render* R)
{
    struct primitive_render_context* context = render_get_context(R, PRIMITIVE_RENDER);

    glBindBuffer(GL_ARRAY_BUFFER, context->vbo);
    glBufferData(GL_ARRAY_BUFFER, PRIMITIVE_VERTEX_SIZE * context->offset, context->vertexes, GL_DYNAMIC_DRAW);

    GLuint loc_pos = context->state.loc.position;
    GLuint loc_color = context->state.loc.color;
    glBindVertexArray(context->vao);

    glEnableVertexAttribArray(loc_pos);
    glEnableVertexAttribArray(loc_color);
    glVertexAttribPointer(loc_pos, 2, GL_FLOAT, GL_FALSE, PRIMITIVE_VERTEX_SIZE, PRIMITIVE_VERTEX_OFFSET_POS);
    glVertexAttribPointer(loc_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, PRIMITIVE_VERTEX_SIZE, PRIMITIVE_VERTEX_OFFSET_COLOR);
    CHECK_GL_ERROR;

    int n = context->current_batch_index;

    for (int i = 0; i < n; ++i) {
        struct primitive_render_batch* b = context->batches + i;
        switch (b->type) {
            case PRIMITIVE_LINE:
            {
                glDrawArrays(GL_LINES, b->offset, b->line_count*2);
                CHECK_GL_ERROR;
                R->drawcall++;
            }
                break;
                
            default:
                break;
        }
        primitive_render_batch_reset(b);
    }

    context->current_batch_index = 0;
    context->offset = 0;
}

void primitive_render_func_draw(struct render* R, enum primitive_type primitive_type, void* object)
{
    static int vertex_size[PRIMITVE_MAX] = {
        4, // line takes 4 floats
        8  // rect takes 8 floats
    };

    struct primitive_render_context* context = render_get_context(R, PRIMITIVE_RENDER);
    if (context->current_batch_index >= MAX_RENDER_BATCH ||
        context->offset + vertex_size[primitive_type] >= VERTICE_SIZE) {
        primitive_render_func_flush(R);
    }

    struct primitive_vertex* buffer = context->vertexes;
    int offset = context->offset;
    struct primitive_vertex* data = buffer + context->offset;

    struct sprite* sprite = (struct sprite*) object;
    struct primitive_render_batch* batch = context->batches + context->current_batch_index;

    switch (primitive_type) {
        case PRIMITIVE_LINE:
            {
                struct primitive_vertex* v = sprite->primitive_vertex;
                data[0] = v[0];
                data[1] = v[1];
                data[2] = v[2];
                data[3] = v[3];

                if (context->state.last_type != PRIMITIVE_LINE ||
                    context->state.line_width != sprite->line_width) {
                    batch->line_width = sprite->line_width;
                    batch->offset = offset;
                    batch->type = PRIMITIVE_LINE;
                    batch->line_count = 1;

                    context->state.line_width = sprite->line_width;
                    context->current_batch_index++;

                } else {
                    batch->offset += 2;
                    batch->line_count++;
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

void primitive_render_func_end(struct render* R)
{

}

void primitive_render_func_init(struct render* R)
{
    struct primitive_render_context* context = STRUCT_NEW(primitive_render_context);
    glGenVertexArrays(1, &context->vao);
    glBindVertexArray(context->vao);
    
    glGenBuffers(1, &context->vbo);

    context->state.last_type = PRIMITIVE_INVALID;
    context->state.line_width = -1;
    context->state.program = -1;
    context->current_batch_index = 0;
    context->n_objects = 0;
    context->vertexes = s_malloc(PRIMITIVE_VERTEX_SIZE * VERTICE_SIZE);
    memset(context->vertexes, 0, PRIMITIVE_VERTEX_SIZE * VERTICE_SIZE);

    memset(context->batches, 0, MAX_RENDER_BATCH*sizeof(struct primitive_render_batch));

    struct render_func func = {
        primitive_render_func_init,
        primitive_render_func_start,
        primitive_render_func_end,
        primitive_render_func_flush,
        NULL,
    };
    struct render_object object = {
        PRIMITIVE_RENDER,
        func,
        context
    };
    render_set_object(R, &object);
    
    CHECK_GL_ERROR;
}