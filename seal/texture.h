#ifndef __seal__texture__
#define __seal__texture__

#include <OpenGL/gl3.h>

struct array_map;
struct texture;
struct Hashmap;

struct texture_cache {
    struct Hashmap* cache;
};

struct texture_cache* texture_cache_new();
void texture_cache_free(struct texture_cache* self);

struct texture* texture_cache_load(struct texture_cache* self, const char* key);
void texture_cache_unload(struct texture_cache* self, const char* key);

struct texture {
    GLuint id;
    unsigned int width, height;
    
    int cursor_x, cursor_y;
    int row_height;
};

struct texture* texture_load_from_png(const char* file_path);

struct texture* texture_load_from_mem(const unsigned char* memory,
                                      unsigned int width,
                                      unsigned int height, GLint mode);

void texture_set_row_height(struct texture* self, unsigned int row_height);

void texture_append(struct texture* self,
                    const unsigned char* memory,
                    unsigned int w,
                    unsigned int h,
                    GLint mode);

void texture_unload(struct texture* self);
//------------------------------------------------------------


#endif
