#ifndef __seal__seal__
#define __seal__seal__

#ifdef DEBUG
#define s_malloc(size) seal_malloc(size, __FILE__, __LINE__)
#define s_calloc(size) seal_calloc(size, __FILE__, __LINE__)
#define s_free(p) do{ seal_free(p); p = NULL;}while(0)

#else
#define s_malloc malloc
#define s_calloc calloc
#define s_free free
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include <float.h>
#include <stddef.h>


#include <OpenGL/gl3.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "base/list.h"

#include "memory.h"
#include "shader.h"
#include "vertex.h"
#include "sprite.h"

#include "platform/fs.h"
#include "platform/timer.h"

struct game {
    lua_State* lstate;
    int window_width;
    int window_height;
};

#define GAME_TRACE  "GAME_TRACE"
#define GAME_UPDATE "GAME_UPDATE"
#define GAME_DRAW   "GAME_DRAW"
#define GAME_PAUSE  "GAME_PAUSE"
#define GAME_RESUME "GAME_RESUME"
#define GAME_EVENT  "GAME_EVENT"

// seal system
void seal_init();
void seal_load_string(const char* script_data);
void seal_load_file(const char* script_path);
void seal_start_game();
void seal_main_loop();
void seal_update(float dt);
void seal_draw();
void seal_destroy();

#endif