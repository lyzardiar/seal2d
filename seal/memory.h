#ifndef __seal__memory__
#define __seal__memory__

#include "seal.h"
#define STRUCT_NEW(type) (struct type*)z_malloc(sizeof(struct type))

extern void* seal_malloc(unsigned long size, const char* file, int line);
extern void seal_free(void* p);
extern void seal_dump_memory();


#endif
