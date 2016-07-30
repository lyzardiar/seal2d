#include "platform/platform.h"

#ifdef PLAT_DESKTOP

#include "memory.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION

#include "nuklear.h"
#define NK_GLFW_GL3_IMPLEMENTATION
#include "nuklear_glfw_gl3.h"

#include "nuk_node.h"
#include "seal.h"
#include "lua_handler.h"
#include "window.h"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

EXTERN_GAME;

static struct nk_context* ctx = NULL;

void nuk_node_ctx_init() {
    struct nk_context* context = nk_glfw3_init(GAME->window->ctx, NK_GLFW3_INSTALL_CALLBACKS);
    ctx = context;
    struct nk_font_atlas *atlas = NULL;
    nk_glfw3_font_stash_begin(&atlas);
    nk_font_atlas_add_from_file(atlas, GAME->config.nk_gui_font_path, GAME->config.nk_gui_font_size, 0);
    nk_glfw3_font_stash_end();
}

struct nk_context* global_nk_context() {
    return ctx;
}

struct nuk_node* nuk_node_new() {
    struct nuk_node* node = STRUCT_NEW(nuk_node);
    node->panel = STRUCT_NEW(nk_panel);
    
    return node;
}

void nuk_node_free(struct nuk_node* self) {
    s_free(self->panel);
    s_free(self);
}

void nuk_draw_start() {
    nk_glfw3_new_frame();
}

void nuk_node_draw(struct nuk_node* self) {
    // we do all the draw stuff in Lua Layer, make the world easier. :)
    lua_handler_exe_func(GAME->lua_handler, GAME->lstate, self, NULL, NULL);
}

void nuk_draw_end() {
    nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
}

#endif