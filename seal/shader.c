#include "shader.h"

void check_gl_error(const char* file, int line) {
    GLenum err = GL_NO_ERROR;
    if((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "gl error: %04x. file = %s, line = %d\n", err, file, line);
    }
}
//
//GLuint craete_shader_from_file(GLenum shader_type, const char* file_path) {
//    SDL_RWops *file = SDL_RWFromFile(file_path, "r");
//    GLuint shader = 0;
//    if (file) {
//        size_t size = 0;
//        if (SDL_RWseek(file, 0, RW_SEEK_END) != -1) {
//            size = SDL_RWtell(file);
//            printf("Final position in stream: %ld\n", size);
//        }
//        
//        GLubyte* buffer = SDL_malloc(size+1);
//        SDL_RWseek(file, 0, RW_SEEK_SET);
//        size_t bytes = file->read(file, buffer, sizeof (buffer), size);
//        if (bytes > 0) {
//            buffer[size] = 0;
//            shader = create_shader(shader_type, (const char*)buffer);
//        } else {
//            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
//                         "craete_shader_from_file failed with path = %s\n", file_path);
//            SDL_assert(SDL_FALSE);
//        }
//        SDL_free(buffer);
//        file->close(file);
//    }
//    
//    return shader;
//}

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