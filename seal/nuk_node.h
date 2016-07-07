//
//  nuk_node.h
//  yuusha
//
//  Created by jukka on 16/7/7.
//  Copyright © 2016年 levelmax. All rights reserved.
//

#ifndef nuk_node_h
#define nuk_node_h

struct nk_context;
struct nk_panel;

struct nuk_node {
    struct nk_panel* panel;
    
    
};

void nuk_node_ctx_init();

struct nuk_node* nuk_node_new();
void nuk_node_free(struct nuk_node* self);

void nuk_draw_start();

void nuk_node_draw(struct nuk_node* self);

void nuk_draw_end();

#endif /* nuk_node_h */
