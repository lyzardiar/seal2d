#ifndef __seal__memory__
#define __seal__memory__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
// memory functions
#define s_malloc(size)          seal_malloc(size, __FILE__, __LINE__)
#define s_calloc(count, size)          seal_calloc(count, size, __FILE__, __LINE__)
#define s_realloc(p, size)      seal_realloc(p, size, __FILE__, __LINE__)
#define s_free(p)               seal_free((p))

#else

#define s_malloc    malloc
#define s_calloc    calloc
#define s_realloc   realloc
#define s_free      free

#endif


#define STRUCT_NEW(type) (struct type*)s_malloc(sizeof(struct type))

void add_entry(void* ptr, size_t size, const char* file, int line);
void delete_entry(void* ptr);

extern void* seal_malloc(size_t size, const char* file, int line);
extern void* seal_calloc(size_t size, const char* file, int line);
extern void* seal_realloc(void* ptr, size_t size, const char* file, int line);
extern void seal_free(void* ptr);
extern void seal_dump_memory();

#endif
