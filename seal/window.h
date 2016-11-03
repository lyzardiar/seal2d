/*
 * Copyright (C) 2016 Tang Yiyang
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See BELOW for details.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#ifndef __yuusha__window__
#define __yuusha__window__

#include "seal_base.h"

// WC - todo fix
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
