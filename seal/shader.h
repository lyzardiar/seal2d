#ifndef __seal__shader__
#define __seal__shader__

#include "platform/render_opengl.h"

#define CHECK_GL_ERROR check_gl_error(__FILE__, __LINE__);

enum SHADER_TYPE {
    SHADER_COLOR = 0,
    SHADER_PRIMITIVE = 1,
    
    MAX_SHADER = 16,
};

enum UNIFORM_ATTRIBUTE_TYPE {
    UNIFORM_1F = 0,
    UNIFORM_2F,
    UNIFORM_3F,
    UNIFORM_4F,
};

enum BUILT_IN_UNIFORMS {
    BUILT_IN_MIX_COLOR,

    BUILT_IN_UNIFORM_MAX,
};

struct builtin_uniform {
    enum BUILT_IN_UNIFORMS type;
    enum UNIFORM_ATTRIBUTE_TYPE attr_type;
    const char* name;
};

struct uniform_buffer_object {
    enum BUILT_IN_UNIFORMS type;
    float v[4];
};

/* it looks like we don't need this function,
 cause the object is managed by the render's array to clean it up.*/

//void uniform_buffer_object_free(struct uniform_buffer_object* self);

struct shader {
    GLuint shader_programs[MAX_SHADER];
    struct builtin_uniform uniforms[BUILT_IN_UNIFORM_MAX];
    
    struct uniform_buffer_object uniform_buffer_objects[BUILT_IN_UNIFORM_MAX];
};

void check_gl_error(const char* file, int line);

struct shader* shader_new();
void shader_free(struct shader* self);

// set the pre-allocated uniform object.
void shader_set_uniform_object(struct shader* self,
                               enum BUILT_IN_UNIFORMS type,
                               float* v);

void shader_commit_uniform(struct shader* self, GLint program);
void shader_set_uniform(struct shader* self, GLint program, enum BUILT_IN_UNIFORMS type, void* v);

GLuint shader_get_program(struct shader* self, enum SHADER_TYPE shader_index);


#endif
