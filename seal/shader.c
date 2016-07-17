#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <OpenGL/gl3.h>

#include "shader.h"
#include "memory.h"
#include "render.h"

#include "platform/fs.h"

#define set_builtin_uniform(uniform, i, t, n) uniform.type = i; \
                                                      uniform.attr_type = t; \
                                                      uniform.name = n; \

void shader_set_uniform_object(struct shader* self,
                               enum BUILT_IN_UNIFORMS type,
                               float* v) {
    
    struct uniform_buffer_object* object = &(self->uniform_buffer_objects[type]);
    
    enum UNIFORM_ATTRIBUTE_TYPE attr_type = self->uniforms[type].attr_type;
    switch (attr_type) {
        case UNIFORM_1F:
            object->v[0] = v[0];
            break;
        case UNIFORM_2F:
            object->v[0] = v[0];
            object->v[1] = v[1];
            break;
        case UNIFORM_3F:
            object->v[0] = v[0];
            object->v[1] = v[1];
            object->v[2] = v[2];
            break;
        case UNIFORM_4F:
            object->v[0] = v[0];
            object->v[1] = v[1];
            object->v[2] = v[2];
            object->v[3] = v[3];
            break;
         default:
            break;
    }
}


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
        "res/shaders/color.vert",
        "res/shaders/color.frag",
        
        "res/shaders/text.vert",
        "res/shaders/text.frag",
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
    
    set_builtin_uniform(shader->uniforms[BUILT_IN_MIX_COLOR], BUILT_IN_MIX_COLOR, UNIFORM_4F, "mix_color");
    
    return shader;
}

void shader_free(struct shader* self) {
    s_free(self);
}

void shader_set_uniform(struct shader* self, GLint program, enum BUILT_IN_UNIFORMS type, void* v) {
    const char* name = self->uniforms[type].name;
    
    GLint location = glGetUniformLocation(program, name);
    
    switch (self->uniforms[type].attr_type) {
        case UNIFORM_1F:
            glUniform1f(location, ((float*)v)[0]);
            break;
        case UNIFORM_2F:
            glUniform2f(location, ((float*)v)[0], ((float*)v)[1]);
            break;
        case UNIFORM_3F:
            glUniform3f(location, ((float*)v)[0], ((float*)v)[1], ((float*)v)[2]);
            break;
        case UNIFORM_4F:
            glUniform4f(location, ((float*)v)[0], ((float*)v)[1], ((float*)v)[2], ((float*)v)[3]);
            break;
        default:
            break;
    }
    CHECK_GL_ERROR;
}

void shader_commit_uniform(struct shader* self, GLint program) {
    for (int i = 0; i < BUILT_IN_UNIFORM_MAX; ++i) {
        shader_set_uniform(self, program, i, self->uniform_buffer_objects[i].v);
    }
}

GLuint shader_get_program(struct shader* self, enum SHADER_TYPE shader_index) {
    assert(shader_index >= 0 && shader_index < MAX_SHADER);
    return self->shader_programs[shader_index];
}
