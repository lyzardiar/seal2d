#include "seal.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <float.h>
#include <unistd.h>

#include "timer.h"
#include "window.h"

#include "GLFW/glfw3.h"

static void
_glfw_error_cb(int error, const char* desc) {
    printf("glfw: %s\n", desc);
}

void set_title(GLFWwindow* window, float dt) {
    static int counter = 0;
    counter++;
    counter = counter % 10;
    if (counter == 0) {
        char title[128] = "";
        if (dt < FLT_EPSILON) {
            dt = 1.0f/60.0f;
        }
        snprintf(title, 128, "%.2f", 1.0f/dt);
        glfwSetWindowTitle(window, title);
    }
}

GLFWwindow* init_glfw(int window_width, int window_height, const char* title) {
    GLFWwindow* window;
    glfwSetErrorCallback(_glfw_error_cb);

    /* Initialize the library */
    if (!glfwInit()) {
        fprintf(stderr, "glfwInit failed.\n");
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window = glfwCreateWindow(window_width, window_height, title, NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "Error, glfwCreateWindow failed.\n");
        glfwTerminate();
        exit(-1);
    }

    glfwSetMouseButtonCallback(window, glfw_mouse_button_callback);
    glfwSetCursorPosCallback(window, glfw_mouse_pos_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    return window;
}

void exit_glfw(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}
//
//#include "render.h"
//#include "shader.h"
//EXTERN_GAME;

int main(int argc, char *argv[]) {
    struct game* game = seal_load_game_config();
    
    int window_width = game->config.window_width;
    int window_height = game->config.window_height;
    
    long interval = (1/60.0f) * 1000;
    long dt = interval;
    long last = 0;
    
    GLFWwindow* window = init_glfw(window_width,
                                   window_height,
                                   game->config.app_name);
    game->window->ctx = window;
    
    seal_init_graphics();
    seal_start_game();

//    float vertex[4] = {0.0f, 0.0f, 1.0f, 1.0f};
//    GLuint vbo, vao;
//    CHECK_GL_ERROR;
//    glGenBuffers(1, &vbo);
//    CHECK_GL_ERROR;
//    glBindBuffer(GL_ARRAY_BUFFER, vbo);
//    CHECK_GL_ERROR;
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
//    CHECK_GL_ERROR;
//
//    glGenVertexArrays(1, &vao);
//    glBindVertexArray(vao);


    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        last = gettime();

//        glClearColor(0.3, 0.3, 0.3, 1.0);
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        GLuint program = shader_get_program(GAME->render->shader, SHADER_PRIMITIVE);
//        glUseProgram(program);
//
//        glBindBuffer(GL_ARRAY_BUFFER, vbo);
//
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float), 0);
//
//        glDrawArrays(GL_LINES, 0, 2);
//        CHECK_GL_ERROR;

        seal_update();
        seal_draw();
        glfwSwapBuffers(window);

        long current = gettime();
        dt = current - last;
        
        if (dt < interval) {
            usleep( (useconds_t)(interval - dt)*1000);
        }
    }
    
    seal_destroy();
    
    exit_glfw(window);
    return 0;
}
