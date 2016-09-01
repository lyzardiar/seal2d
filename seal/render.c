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
/*-------------------- render --------------------*/

#define RENDER_INVALID (-1)

extern void sprite_render_func_init(struct render* R);
extern void primitive_render_func_init(struct render* R);

struct render* render_new()
{
    struct render* r = STRUCT_NEW(render);
    r->last = r->current = RENDER_INVALID;

    sprite_render_func_init(r);
    primitive_render_func_init(r);

    r->shader = shader_new();
    return r;
}

void render_set_object(struct render* self, struct render_object* object)
{
    memcpy(&self->R_objs[object->type], object, sizeof(*object));
}

void* render_get_context(struct render* self, enum RENDER_TYPE render_object_type)
{
    return self->R_objs[render_object_type].context;
}

void render_free(struct render* self)
{
    shader_free(self->shader);
    s_free(self);
}

void render_clear(struct render* self, color c)
{
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

void render_switch(struct render* self, enum RENDER_TYPE type)
{
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

void render_set_mvp(GLuint program, float* mat)
{
    GLint projection = glGetUniformLocation(program, "mvp");
    glUniformMatrix4fv(projection, 1, GL_FALSE, mat);
}