#include "seal.h"

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

GLFWwindow* init_glfw() {
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
    
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        fprintf(stderr, "Error, glfwCreateWindow failed.\n");
        glfwTerminate();
        exit(-1);
    }
    glfwSetErrorCallback(_glfw_error_cb);
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    
    return window;
}

void exit_glfw(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main(int argc, char *argv[]) {

    GLFWwindow* window = init_glfw();
    
    seal_init();

    long interval = (1/60.0f) * 1000;
    float dt = interval;
    while (!glfwWindowShouldClose(window))
    {
        long last = gettime();

        glfwPollEvents();

        seal_update(dt);
        seal_draw();
        
        set_title(window, dt);
        glfwSwapBuffers(window);

        long current = gettime();
        
        dt = (current - last)/1000.0f;
        printf("dt = %.4f\n", dt);
        if (current - last < interval) {
            usleep( (useconds_t)(interval - current + last)*1000 );
        }
    }
    
    seal_destroy();
    
    exit_glfw(window);
    return 0;
}
