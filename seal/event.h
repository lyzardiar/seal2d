#ifndef __seal__event__
#define __seal__event__

#include <stdbool.h>
#include "platform/platform.h"

#ifdef PLAT_DESKTOP
#include <GLFW/glfw3.h>
#endif

#define TOUCH_BEGIN  (0)
#define TOUCH_MOVE   (1)
#define TOUCH_END    (2)
#define TOUCH_CANCEL (3)

#define SPRITE_EVENT_TYPE "touch"

struct touch_event {
    int type;
    int x, y;
    bool responsed;
    bool swallowd;
};

struct key_event {
    int key;
    int action;
};

#endif
