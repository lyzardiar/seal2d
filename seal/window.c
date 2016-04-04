#include "seal.h"

#include "memory.h"
#include "event.h"

#include "window.h"

EXTERN_GAME;
struct window* win_alloc() {
    struct window* win = STRUCT_NEW(window);
    win->cursor_x = 0.0f;
    win->cursor_y = 0.0f;
    win->is_touch_down = 0;
    return win;
}

void win_free(struct window* win) {
    s_free(win);
}

#ifdef PLAT_DESKTOP
void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int modify) {
    switch (action) {
        case GLFW_PRESS: {
                struct event e = {TOUCH_MOVE,
                                 GAME->window->cursor_x,
                                 GAME->window_height - GAME->window->cursor_y};
                seal_event(&e);
                GAME->window->is_touch_down = 1;
            }
            break;
            
        case GLFW_RELEASE: {
                struct event e = {TOUCH_END,
                    GAME->window->cursor_x,
                    GAME->window_height - GAME->window->cursor_y};
                seal_event(&e);
                GAME->window->is_touch_down = 0;
            }
            break;
            
        default:
            break;
    }
}

void glfw_mouse_pos_callback(GLFWwindow* window, double x, double y) {
    struct window* win = GAME->window;
    win->cursor_x = x;
    win->cursor_y = y;
    if (win->is_touch_down) {
        struct event e = {TOUCH_MOVE, x, y};
        seal_event(&e);
    }
}

#endif