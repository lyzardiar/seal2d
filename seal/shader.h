#ifndef __seal__shader__
#define __seal__shader__

#include <OpenGL/gl3.h>

#define CHECK_GL_ERROR check_gl_error(__FILE__, __LINE__);

enum SHADER_TYPE {
    SHADER_COLOR = 0,
    SHADER_TEXT = 1,
    
    MAX_SHADER = 64,
};

struct shader {
    GLuint shader_programs[MAX_SHADER];
};

void check_gl_error(const char* file, int line);

struct shader* shader_new();
void shader_free(struct shader* self);

GLint shader_get_uniform(struct shader* self, GLint program, const char* name);

GLuint shader_get_program(struct shader* self, enum SHADER_TYPE shader_index);


#endif
