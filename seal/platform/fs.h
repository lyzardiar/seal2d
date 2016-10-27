#ifndef __seal__fs__
#define __seal__fs__

#include "platform.h"
// user's duty to free
// extra_byte: 0 or 1, if you were to read a string, we ensure another byte
// which is set '\0' to make a c-string.

const char* fs_full_path(const char* filename);
const char* fs_sandbox_root_path();

unsigned char* fs_read(const char* path, size_t* size, int extra_byte);
char* fs_reads(const char* path);

size_t fs_writes(const char* path, const char* string);

size_t fs_writef(const char* path, const void* data, size_t size);

const char* fs_get_write_path();


#endif /* defined(__zore__zore_fs__) */
