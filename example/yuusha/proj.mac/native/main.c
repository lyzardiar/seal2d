#include "seal.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <float.h>
#include <unistd.h>

#include "GLFW/glfw3.h"

#include "timer.h"
#include "window.h"

#ifdef UNIT_TEST
#include "tests/unit_test/test.h"
void run_unit_tests() {
    //test_array_map();
//    test_array();
    test_2d_array();
}
#endif

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

    glfwSetErrorCallback(_glfw_error_cb);
    glfwMakeContextCurrent(window);
    
    glfwSwapInterval(1);
    return window;
}

void exit_glfw(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main(int argc, char *argv[]) {
    struct game* game = seal_load_game_config();
    

    GLFWwindow* window = init_glfw(game->window_width, game->window_height, game->app_name);
#ifdef UNIT_TEST
    run_unit_tests();
#endif
    
    seal_init_graphics();

    float interval = (1/60.0f) * 1000;
    float dt = interval;
    long last = 0;
    
    struct timeval _lastUpdate;
    gettimeofday(&_lastUpdate, NULL);
    
    while (!glfwWindowShouldClose(window))
    {
        
        last = gettime();
        glfwPollEvents();
        
        struct timeval now;
        gettimeofday(&now, NULL);
        
        // delta只计算draw的时间
        float delta = ((now.tv_sec - _lastUpdate.tv_sec)/1000.0f + (now.tv_usec - _lastUpdate.tv_usec) * 1000.0f)/1000000000.0f;
        seal_update(delta);
        seal_draw();
        set_title(window, delta);
        
        glfwSwapBuffers(window);
        
        _lastUpdate = now;
        long current = gettime();

        dt = (current - last)/1000.0f;
        last = current;
        if (dt < interval) {
            usleep( (useconds_t)(interval - dt)*1000 );
        }
    }
    
    seal_destroy();
    
    exit_glfw(window);
    return 0;
}
