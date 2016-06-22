#include "seal.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <float.h>
#include <unistd.h>

#include "timer.h"
#include "window.h"

#include "GLFW/glfw3.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear/nuklear.h"
#include "nuklear/nuklear_glfw_gl3.h"

#ifdef __APPLE__
#	define GLFW_INCLUDE_GLCOREARB
#endif
#include "nanovg.h"
#define NANOVG_GL3_IMPLEMENTATION
#include "nanovg_gl.h"


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

int main(int argc, char *argv[]) {
    struct game* game = seal_load_game_config();
    
    int window_width = game->config.window_width;
    int window_height = game->config.window_height;
    
    long interval = (1/60.0f) * 1000;
    long dt = interval;
    long last = 0;
    long start = gettime();
    
    GLFWwindow* window = init_glfw(window_width,
                                   window_height,
                                   game->config.app_name);
    
    /************************ nuklear context begin ************************/
    struct nk_context* ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
    game->nk_gui_ctx = ctx;
    
    seal_init_graphics();
    seal_start_game();
    
    // init the nklear font here.
    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    nk_font_atlas_add_from_file(atlas, game->config.nk_gui_font_path, game->config.nk_gui_font_size, 0);
    nk_glfw3_font_stash_end();
    /************************ nuklear context end ************************/
    
    /************************ nanovg begin ************************/
    NVGcontext* vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
    float pxRatio = (float)window_width / (float)window_height;
    /************************ nanovg end ************************/
    
 

    
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        last = gettime();
        seal_update(vg);
        seal_draw(window);

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
