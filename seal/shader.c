#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "platform/render_opengl.h"

#include "shader.h"
#include "memory.h"
#include "render.h"

#include "platform/fs.h"


#ifdef PLAT_DESKTOP
//-------------------------- normal sprite shader for desktop --------------------------
static const char* vs_sprite = STRINGFY(#version 330\n)STRINGFY(\n
    layout(location = 0) in vec2 vertex_pos; \n
    layout(location = 1) in vec4 vertex_color; \n
    layout(location = 2) in vec2 vertex_uv; \n
    out vec4 frag_color;\n
    out vec2 frag_uv;\n
    uniform mat4 mvp;\n

    void main() {\n
       gl_Position.xy = (mvp * vec4(vertex_pos.x, vertex_pos.y, 0.0f, 1.0f)).xy;\n
       gl_Position.z = 0.0;\n
       gl_Position.w = 1.0;\n
       frag_color = vertex_color;\n
       frag_uv = vec2(vertex_uv.x, 1.0f - vertex_uv.y);\n
    }\n
);

static const char* fs_sprite = STRINGFY(#version 330\n)STRINGFY(
    in vec4 frag_color;\n
    in vec2 frag_uv;\n
    out vec4 color;\n
    uniform sampler2D texture_0;\n

    void main() {\n
       vec4 texture_color = texture(texture_0, frag_uv);\n
       color = texture_color * frag_color;\n
    }\n
    );

//-------------------------- normal primitive shader for desktop -----------------------

static const char* vs_primitive = STRINGFY(#version 330\n)STRINGFY(\n
    layout(location = 0) in vec2 vertex_pos; \n
    layout(location = 1) in vec4 vertex_color; \n
    out vec4 frag_color;\n
    uniform mat4 mvp;\n
                                                                   
    void main() {\n
       gl_Position.xy = (mvp * vec4(vertex_pos.x, vertex_pos.y, 0.0f, 1.0f)).xy;\n
       gl_Position.z = 0.0;\n
       gl_Position.w = 1.0;\n
       frag_color = vertex_color;\n
    }\n
    );

static const char* fs_primitive = STRINGFY(#version 330\n)STRINGFY(
   in vec4 frag_color;\n
   out vec4 color;\n
   
   void main() {\n
       color = frag_color;\n
   }\n
   );
#endif

#ifdef PLAT_MOBILE
static const char* vs_sprite = STRINGFY(\n
    precision lowp float;\n
    attribute mediump vec2 vertex_pos;\n
    attribute lowp vec4 vertex_color;\n
    attribute mediump vec2 vertex_uv; \n\n
    uniform mat4 mvp; \n

    varying lowp vec4 fragement_color;\n
    varying mediump vec2 fragement_uv; \n\n
    void main() {\n
        gl_Position = mvp * vec4(vertex_pos.x, vertex_pos.y, 0.0, 1.0); \n
        fragement_color = vertex_color;\n
        fragement_uv = vec2(vertex_uv.x, 1.0 - vertex_uv.y); \n
    }\n
    );

static const char* fs_sprite = STRINGFY(\n
    precision lowp float;\n
    varying lowp vec4 fragement_color; \n
    varying mediump vec2 fragement_uv; \n\n

    uniform sampler2D texture_0; \n\n
    void main() {\n
        gl_FragColor = texture2D(texture_0, fragement_uv) * fragement_color; \n
    }\n
    );

static const char* vs_primitive = STRINGFY(\n
    precision lowp float;\n
    attribute mediump vec2 vertex_pos;\n
    attribute lowp vec4 vertex_color;\n
    uniform mat4 mvp; \n
    varying lowp vec4 fragement_color;\n

    void main() {\n
        gl_Position = mvp * vec4(vertex_pos.x, vertex_pos.y, 0.0, 1.0); \n
        fragement_color = vertex_color;\n
    }\n
    );

static const char* fs_primitive = STRINGFY(\n
    precision lowp float;\n
    varying lowp vec4 fragement_color; \n

    void main() {\n
        gl_FragColor = fragement_color;
    }\n
    );

#endif

#define set_builtin_uniform(uniform, i, t, n) uniform.type = i; \
                                                      uniform.attr_type = t; \
                                                      uniform.name = n; \

void shader_set_uniform_object(struct shader* self,
                               enum BUILT_IN_UNIFORMS type,
                               float* v)
{    
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

void check_gl_error(const char* file, int line)
{
    GLenum err = GL_NO_ERROR;
    if((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "gl error: %04x. file = %s, line = %d\n", err, file, line);
    }
}

static GLuint create_program(GLuint vs, GLuint fs)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    
    glLinkProgram(program);
    
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
    glDetachShader(program, fs);

    CHECK_GL_ERROR;
    return program;
}

static GLuint create_shader(GLenum shader_type, const char* shader_data)
{
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_data, NULL);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
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
        
        fprintf(stderr, "Compile failure in %s shader:\n%s\n shader_src = %s",
                strShaderType, strInfoLog, shader_data);
    }
    
    return shader;
}

static GLuint craete_shader_from_file(GLenum shader_type, const char* file_path)
{
    char* bytes = fs_reads(file_path);
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

static void shader_load_all(struct shader* self)
{
    const char* shaders[] = {
        vs_sprite,
        fs_sprite,

        vs_primitive,
        fs_primitive,
    };

    int n = sizeof(shaders)/sizeof(const char*)/2;
    for (int i = 0; i < n; ++i) {
        int index = i*2;
        GLuint vs = create_shader(GL_VERTEX_SHADER, shaders[index]);
        GLuint fs = create_shader(GL_FRAGMENT_SHADER, shaders[index+1]);
        
        GLuint program = create_program(vs, fs);
        self->shader_programs[i] = program;
    }
}

struct shader* shader_new()
{
    struct shader* shader = STRUCT_NEW(shader);
    memset(shader->shader_programs, 0, MAX_SHADER*sizeof(GLuint));
    
    shader_load_all(shader);
    
    set_builtin_uniform(shader->uniforms[BUILT_IN_MIX_COLOR], BUILT_IN_MIX_COLOR, UNIFORM_4F, "mix_color");
    
    return shader;
}

void shader_free(struct shader* self)
{
    s_free(self);
}

void shader_set_uniform(struct shader* self, GLint program, enum BUILT_IN_UNIFORMS type, void* v)
{
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

void shader_commit_uniform(struct shader* self, GLint program)
{
    for (int i = 0; i < BUILT_IN_UNIFORM_MAX; ++i) {
        shader_set_uniform(self, program, i, self->uniform_buffer_objects[i].v);
    }
}

GLuint shader_get_program(struct shader* self, enum SHADER_TYPE shader_index)
{
    assert(shader_index >= 0 && shader_index < MAX_SHADER);
    return self->shader_programs[shader_index];
}
