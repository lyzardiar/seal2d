#ifndef __test__shader__
#define __test__shader__

#include "seal.h"

#define CHECK_GL_ERROR check_gl_error(__FILE__, __LINE__);

void setup_opengl(SDL_Window* window);
GLuint craete_shader_from_file(GLenum shader_type, const char* file_path);
GLuint create_shader(GLenum shader_type, const char* shader_data);
GLuint create_program(GLuint vs, GLuint fs);


#endif /* defined(__test__shader__) */
