#ifndef __yuusha__window__
#define __yuusha__window__

#include "platform/platform.h"

#ifdef PLAT_DESKTOP
    #include <GLFW/glfw3.h>
#endif

struct window* win_alloc();
void win_free(struct window* win);

#ifdef PLAT_DESKTOP

struct window {
    double cursor_x, cursor_y;
    int is_touch_down; // bool, if the user is touching the screen.
    
    void *ctx; // varies depend on platform, reffers to GLFWindow* if we were on mac.
};

void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int modify);
void glfw_mouse_pos_callback(GLFWwindow* window, double x, double y);
void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif


#endif /* defined(__yuusha__window__) */
