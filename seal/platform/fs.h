#ifndef __seal__fs__
#define __seal__fs__

#include "seal.h"

// user's duty to free
void* s_read(const char* path, size_t* size, int prefer_external);

size_t s_writes(const char* path, const char* string);

size_t s_writef(const char* path, const void* data, size_t size);


#endif /* defined(__zore__zore_fs__) */
