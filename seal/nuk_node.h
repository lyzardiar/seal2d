#ifndef nuk_node_h
#define nuk_node_h

#include "platform/platform.h"

#ifdef PLAT_DESKTOP

struct nk_context;
struct nk_panel;

struct nuk_node {
    struct nk_panel* panel;
};

struct nk_context* global_nk_context();

void nuk_node_ctx_init();

struct nuk_node* nuk_node_new();
void nuk_node_free(struct nuk_node* self);

void nuk_draw_start();

void nuk_node_draw(struct nuk_node* self);

void nuk_draw_end();

#endif

#endif