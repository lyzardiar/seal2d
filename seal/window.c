#include "seal.h"

EXTERN_GAME;
#ifdef PLAT_DESKTOP
struct window* win_alloc()
{
    struct window* win = STRUCT_NEW(window);
    win->cursor_x = 0.0f;
    win->cursor_y = 0.0f;
    win->is_touch_down = 0;
    return win;
}

void win_free(struct window* win)
{
    s_free(win);
}

void glfw_mouse_button_callback(GLFWwindow* window,
                                int button, int action, int modify)
{
    switch (action) {
        case GLFW_PRESS: {
                struct touch_event e = {TOUCH_BEGIN,
                                 GAME->window->cursor_x,
                                 GAME->config.window_height - GAME->window->cursor_y,
                                 false,
                                 false};
                seal_touch_event(&e);
                GAME->window->is_touch_down = 1;
            }
            break;

        case GLFW_RELEASE: {
                struct touch_event e = {TOUCH_END,
                    GAME->window->cursor_x,
                    GAME->config.window_height - GAME->window->cursor_y,
                    false,
                    false};
                seal_touch_event(&e);
                GAME->window->is_touch_down = 0;
            }
            break;

        default:
            break;
    }
}

void glfw_mouse_pos_callback(GLFWwindow* window, double x, double y)
{
    struct window* win = GAME->window;
    win->cursor_x = x;
    win->cursor_y = y;
    if (win->is_touch_down) {
        struct touch_event e = {TOUCH_MOVE,
                                x,
                                GAME->config.window_height - y,
                                false, false};
        seal_touch_event(&e);
    }
}

void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key != GLFW_KEY_UNKNOWN) {
        struct key_event e = {key, action};
        seal_event(EVENT_KEY_RECEIVE, on_seal_key_receive, &e);
    }
}

void glfw_framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
    GAME->config.fb_width = w;
    GAME->config.fb_height = h;

    GAME->config.scale_factor = w/GAME->config.window_width;
}

#endif