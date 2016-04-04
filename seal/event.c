#include <stdio.h>

#include "event.h"

#ifdef PLAT_DESKTOP
void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int modify) {
    printf("mouse: button = %d, action = %d, modify = %d \n", button, action, modify);
}

void glfw_mouse_pos_callback(GLFWwindow* window, double x, double y) {
    printf("pos = %.2f, %.2f \n", x, y);
}

#endif