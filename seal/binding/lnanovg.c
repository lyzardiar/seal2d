#include "platform/platform.h"

#include "lopen.h"
#include "lua.h"
#include "lauxlib.h"

#include "platform/render_opengl.h"
#include "nanovg.h"

#ifdef PLAT_DESKTOP
    #ifdef __APPLE__
        #define GLFW_INCLUDE_GLCOREARB
    #endif

    #define NANOVG_GL3_IMPLEMENTATION
    #include "nanovg_gl.h"
#endif

static NVGcontext* vg_ctx = NULL;
static float px_ratio = 1.0f;
static int win_w = 1024;
static int win_h = 768;
void nanovg_init(int w, int h)
{
    if (!vg_ctx) {
        vg_ctx = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
        win_w = w;
        win_h = h;
        px_ratio = (float)w/(float)h;
    }
}

int lnvg_begin_frame(lua_State* L)
{
    nvgBeginFrame(vg_ctx, win_w, win_h, px_ratio);
    return 0;
}

bind_func_implemention_v(nvg_end_frame, nvgEndFrame, vg_ctx);

// lines
bind_func_implemention_v(nvg_begin_path, nvgBeginPath, vg_ctx);
bind_func_implemention_f2(nvg_move_to, nvgMoveTo, vg_ctx);
bind_func_implemention_f2(nvg_line_to, nvgLineTo, vg_ctx);
bind_func_implemention_i1(nvg_line_cap, nvgLineCap, vg_ctx);
bind_func_implemention_i1(nvg_line_join, nvgLineJoin, vg_ctx);
bind_func_implemention_v(nvg_stroke, nvgStroke, vg_ctx);
bind_func_implemention_f1(nvg_stroke_width, nvgStrokeWidth, vg_ctx);

// state management
bind_func_implemention_v(nvg_save, nvgSave, vg_ctx);
bind_func_implemention_v(nvg_restore, nvgRestore, vg_ctx);
bind_func_implemention_v(nvg_reset, nvgReset, vg_ctx);

int lnvg_stoke_color(lua_State* L)
{
    nvgStrokeColor(vg_ctx,
                   nvgRGBA(luaL_checkinteger(L, 1),
                           luaL_checkinteger(L, 2),
                           luaL_checkinteger(L, 3),
                           luaL_checkinteger(L, 4)));
    return 0;
}

int luaopen_nanovg_core(lua_State* L)
{
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { "begin_frame", lnvg_begin_frame },
        { "end_frame",   lnvg_end_frame },

        { "begin_path", lnvg_begin_path },
        { "move_to", lnvg_move_to },
        { "line_to", lnvg_line_to },
        { "line_cap", lnvg_line_cap },
        { "line_join" ,lnvg_line_join },
        { "stroke", lnvg_stroke },
        { "stroke_width", lnvg_stroke_width },
        { "stroke_color", lnvg_stoke_color},
        { NULL, NULL },
    };

    luaL_newlib(L, lib);

    return 1;
}