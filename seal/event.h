#ifndef __seal__event__
#define __seal__event__

#include "platform/platform.h"

#ifdef PLAT_DESKTOP
#include <GLFW/glfw3.h>
#endif

#define TOUCH_BEGIN  (0)
#define TOUCH_MOVE   (1)
#define TOUCH_END    (2)
#define TOUCH_CANCEL (3)

struct event {
    int type;
    int x, y;
};

#ifdef PLAT_DESKTOP

void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int modify);
void glfw_mouse_pos_callback(GLFWwindow* window, double x, double y);

#endif




#endif
