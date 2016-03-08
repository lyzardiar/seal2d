#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <OpenGL/gl3.h>

#include "shader.h"
#include "memory.h"

#include "platform/fs.h"



static GLuint programes[MAX_SHADER] = {0};

void check_gl_error(const char* file, int line) {
    GLenum err = GL_NO_ERROR;
    if((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "gl error: %04x. file = %s, line = %d\n", err, file, line);
    }
}

void load_shaders() {
    
    GLuint vs = craete_shader_from_file(GL_VERTEX_SHADER, "res/shaders/color.vert");
    GLuint fs = craete_shader_from_file(GL_FRAGMENT_SHADER, "res/shaders/color.frag");
    
    GLuint program = create_program(vs, fs);
    programes[COLOR_SHADER] = program;
}

GLuint get_program(int shader_index) {
    assert(shader_index >= 0 && shader_index < MAX_SHADER);
    GLuint program = programes[shader_index];
    assert(program);
    return program;
}

GLuint craete_shader_from_file(GLenum shader_type, const char* file_path) {
    char* bytes = s_reads(file_path);
    GLuint shader = 0;
    if (bytes > 0) {
        shader = create_shader(shader_type, (const char*)bytes);
    } else {
        fprintf(stderr, "craete_shader_from_file failed with path = %s\n", file_path);
        assert(0);
    }
    s_free(bytes);
    return shader;
}

GLuint create_shader(GLenum shader_type, const char* shader_data) {
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_data, NULL);
    CHECK_GL_ERROR;
    glCompileShader(shader);
    CHECK_GL_ERROR;
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    CHECK_GL_ERROR;
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        GLchar strInfoLog[4096] = "";
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
        
        const char *strShaderType = NULL;
        switch(shader_type) {
            case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
            case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
            default: strShaderType = "unkown"; break;
        }
        
        fprintf(stderr, "Compile failure in %s bshader:\n%s\n", strShaderType, strInfoLog);
    }
    
    return shader;
}

GLuint create_program(GLuint vs, GLuint fs) {
    GLuint program = glCreateProgram();
    CHECK_GL_ERROR;
    glAttachShader(program, vs);
    CHECK_GL_ERROR;
    glAttachShader(program, fs);
    CHECK_GL_ERROR;
    
    glLinkProgram(program);
    CHECK_GL_ERROR;
    
    GLint status;
    glGetProgramiv (program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        GLchar strInfoLog[4096] = "";
        glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
    }
    
    glDetachShader(program, vs);
    CHECK_GL_ERROR;
    glDetachShader(program, fs);
    CHECK_GL_ERROR;
    return program;
}