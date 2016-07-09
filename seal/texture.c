#include <stdio.h>
#include <stdlib.h>

#include "memory.h"

#include "platform/fs.h"
#include "image/lodepng.h"
#include "base/hashmap.h"

#include "util.h"
#include "shader.h"

#include "texture.h"

static int hash_str(void* key) {
    return hashmapHash(key, strlen(key));
}

static bool hash_equal(void* a, void* b) {
    return strcmp(a, b) == 0;
}

struct texture_cache* texture_cache_new() {
    struct texture_cache* cache = STRUCT_NEW(texture_cache);
    cache->cache = hashmapCreate(32, hash_str, hash_equal);
    return cache;
}

void texture_cache_free(struct texture_cache* self) {
    hashmapFree(self->cache);
    s_free(self);
}

// TODO : export load from binary data in Lua.
struct texture* texture_cache_load(struct texture_cache* self, const char* key) {
    struct texture* tex = hashmapGet(self->cache, (void*)key);
    if (!tex) {
        tex = texture_load_from_png(key);
        hashmapPut(self->cache, (void*)key, tex);
    }
    return tex;
}

void texture_cache_unload(struct texture_cache* self, const char* key) {
    struct texture* tex = hashmapGet(self->cache, (void*)key);
    if (!tex) {
        hashmapPut(self->cache, (void*)key, NULL);
        texture_unload(tex);
    } else {
        fprintf(stderr, "texture %s has already been removed", key);
    }
}

struct texture* texture_load_from_png(const char* file_path) {
    if(!file_path) {
        fprintf(stderr, "texure, texture_load_from_png, file_path is nil?");
        return NULL;
    }
    
    // load png data into memory 
    size_t file_size = 0;
    unsigned char* origin_data = s_read(file_path, &file_size, 0);

    unsigned char* pixel_data = NULL;
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int error = 0;
    
    error = lodepng_decode32(&pixel_data, &width, &height, origin_data, file_size);
    s_free(origin_data);
    if(error) {
        fprintf(stderr, "error %u: %s\n", error, lodepng_error_text(error));
        return NULL;
    }
    
    struct texture* tex = texture_load_from_mem(pixel_data, width, height, GL_RGBA);
 
    // TODO: third party use the standard malloc, we may replace that with s_malloc some day.
    free(pixel_data);
    return tex;
}

struct texture* texture_load_from_mem(const unsigned char* pixel,
                                      unsigned int width,
                                      unsigned int height,
                                      GLint mode) {
    // generate opengl texture
    struct texture* tex = s_malloc(sizeof(struct texture));
    tex->id = 0;
    glGenTextures(1, &tex->id);
    s_assert(tex->id != 0);
    
    glBindTexture(GL_TEXTURE_2D, tex->id);
    glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0, mode, GL_UNSIGNED_BYTE, pixel);
    CHECK_GL_ERROR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    CHECK_GL_ERROR
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    
    
    CHECK_GL_ERROR
    tex->width = width;
    tex->height = height;
    return tex;
}

void texture_set_row_height(struct texture* self, unsigned int row_height) {
    self->row_height = row_height;
}

void texture_append(struct texture* self, unsigned char* pixel, unsigned int w, unsigned int h, GLint mode) {
    if (pixel) {
        int nx = self->cursor_x + w;
        if (nx >= self->width) {
            self->cursor_y += self->row_height;
            nx = 0;
        }
        self->cursor_x = nx;
        
        glBindTexture(GL_TEXTURE_2D, self->id);
        CHECK_GL_ERROR;
        glPixelStorei(GL_UNPACK_ALIGNMENT,1);
        CHECK_GL_ERROR;
        glTexSubImage2D(GL_TEXTURE_2D, 0, self->cursor_x, self->cursor_y, w, h, mode, GL_UNSIGNED_BYTE, pixel);
        CHECK_GL_ERROR;
        glBindTexture(GL_TEXTURE_2D, 0);
        CHECK_GL_ERROR;
    }
}

void texture_unload(struct texture* self) {
    glDeleteTextures(1, &self->id);
    s_free(self);
}