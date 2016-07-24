#ifndef render_h
#define render_h

#include <OpenGL/gl3.h>
#include "geo.h"
#include "shader.h"
#include "sprite.h"

#define MAX_OBJECTS (1024) // max objects in on drawcall
#define MAX_RENDER_BATCH (MAX_OBJECTS) // we may use 1 draw call for each of the object

struct render;

enum RENDER_TYPE {
    SPRITE_RENDER = 0,
    
    RENDER_MAX,
};

struct render_func {
    void (*init)(struct render*);
    void (*start)(struct render*);
    void (*draw)(struct render*, void* render_object);
    void (*end)(struct render*);
    void (*flush)(struct render*);
    void (*destroy)(struct render*);
};

struct render_object {
    enum RENDER_TYPE type;
    
    struct render_func render_func;
};

struct render {
    struct render_object R_objs[RENDER_MAX];
    
    struct shader* shader;
    int last;
    int current; // current render type.
    
    int drawcall;
    void* context;
};

struct vertex_buffer {
    GLuint vao;
    GLuint vibo;
    GLuint vbo;
    
    struct vertex* data;
    GLushort* idata;
    int offset;
};

struct render_state {
    int tex_id;
};

struct render_batch {
    int n_objects;
    int offset;
    GLint tex_id;
};

struct sprite_render_context {
    struct vertex_buffer* buffer;
    struct render_batch batches[MAX_RENDER_BATCH];
    struct render_state state;
    
    int current_batch_index;
    int n_objects;
};

struct render* render_new();
void render_free(struct render* self);
void render_clear(struct render* self, color c);

void render_set_context(struct render* self, void* context);
void render_switch(struct render* self, enum RENDER_TYPE type);

void sprite_render_func_draw(struct render* R, void* object);
void sprite_render_func_flush(struct render* R);

#endif
