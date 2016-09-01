#ifndef render_h
#define render_h


#include "geo.h"
#include "shader.h"
#include "sprite.h"

#define MAX_OBJECTS (1024) // max objects in on drawcall
#define MAX_RENDER_BATCH (MAX_OBJECTS) // we may use 1 draw call for each of the object

#define INVALID_ATTR_LOCATION (-1)

struct render;

enum RENDER_TYPE {
    SPRITE_RENDER = 0,
    PRIMITIVE_RENDER,
    
    RENDER_MAX,
};

struct render_func {
    void (*init)(struct render*);
    void (*start)(struct render*);
    void (*end)(struct render*);
    void (*flush)(struct render*);
    void (*destroy)(struct render*);
};

struct render_object {
    enum RENDER_TYPE type;
    
    struct render_func render_func;
    void* context;
};

struct render {
    struct render_object R_objs[RENDER_MAX];
    
    struct shader* shader;
    int last;
    int current; // current render type.
    
    int drawcall;
};

struct vertex_buffer {
    // vertex data
    GLuint vao;
    GLuint vbo;
    struct vertex* data;
    int offset;

    // index buffer
    GLuint vibo;
    GLushort* idata;
};

struct attr_location {
    GLuint position;
    GLuint color;
    GLuint uv;
};

struct render_state {
    int tex_id;
    GLuint program;
    struct attr_location loc;
};

struct render_batch {
    int n_objects;
    int offset;
    GLint tex_id;
};

struct render* render_new();
void render_free(struct render* self);
void render_clear(struct render* self, color c);

void render_set_object(struct render* self, struct render_object* object);
void* render_get_context(struct render* self, enum RENDER_TYPE render_object_type);
void render_switch(struct render* self, enum RENDER_TYPE type);

void render_set_mvp(GLuint program, float* mat);

#endif
