#ifndef __seal__shader__
#define __seal__shader__

#include <OpenGL/gl3.h>

#define CHECK_GL_ERROR check_gl_error(__FILE__, __LINE__);

enum {
    COLOR_SHADER = 0,
    
    MAX_SHADER = 32,
};

void check_gl_error(const char* file, int line);

void load_shaders();

GLuint get_program(int shader_index);

void use_program(GLuint program);
void unused_program(GLuint program);

GLuint craete_shader_from_file(GLenum shader_type, const char* file_path);
GLuint create_shader(GLenum shader_type, const char* shader_data);
GLuint create_program(GLuint vs, GLuint fs);


#endif
