#ifndef __seal__memory__
#define __seal__memory__

#include "seal.h"
#define STRUCT_NEW(type) (struct type*)z_malloc(sizeof(struct type))

void add_entry(void* ptr, size_t size, const char* file, int line);
void delete_entry(void* ptr);

extern void* seal_malloc(size_t size, const char* file, int line);
extern void* seal_calloc(size_t size, const char* file, int line);
extern void* seal_realloc(void* ptr, size_t size, const char* file, int line);
extern void seal_free(void* ptr);
extern void seal_dump_memory();

#endif
