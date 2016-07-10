#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <OpenGL/gl3.h>

#include "shader.h"
#include "memory.h"

#include "platform/fs.h"

void check_gl_error(const char* file, int line) {
    GLenum err = GL_NO_ERROR;
    if((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "gl error: %04x. file = %s, line = %d\n", err, file, line);
    }
}

static GLuint create_program(GLuint vs, GLuint fs) {
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

static GLuint create_shader(GLenum shader_type, const char* shader_data) {
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

static GLuint craete_shader_from_file(GLenum shader_type, const char* file_path) {
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

static void shader_load_all(struct shader* self) {
    const char* shaders[] = {
        "res/shaders/text.vert",
        "res/shaders/text.frag",
        
        "res/shaders/color.vert",
        "res/shaders/color.frag",
    };
    
    int n = sizeof(shaders)/sizeof(const char*)/2;
    for (int i = 0; i < n; ++i) {
        int index = i*2;
        GLuint vs = craete_shader_from_file(GL_VERTEX_SHADER, shaders[index]);
        GLuint fs = craete_shader_from_file(GL_FRAGMENT_SHADER, shaders[index+1]);
        
        GLuint program = create_program(vs, fs);
        self->shader_programs[i] = program;
    }
}

struct shader* shader_new() {
    struct shader* shader = STRUCT_NEW(shader);
    memset(shader->shader_programs, 0, MAX_SHADER*sizeof(GLuint));
    
    shader_load_all(shader);
    return shader;
}

void shader_free(struct shader* self) {
    s_free(self);
}

GLuint shader_get_program(struct shader* self, enum SHADER_TYPE shader_index) {
    assert(shader_index >= 0 && shader_index < MAX_SHADER);
    return self->shader_programs[shader_index];
}
