#ifndef __seal__array__
#define __seal__array__

#include "seal.h"

struct array;

struct array* array_new(int cap);
void array_free(struct array* a);

void array_push_back(struct array* self, void* data);
void array_set(struct array* self, size_t index, void* data);
void* array_at(struct array* self, size_t index);
void array_clear(struct array* self);
int array_empty(struct array* self);

void array_debug_print(struct array* self);

#endif