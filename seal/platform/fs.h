#ifndef __seal__fs__
#define __seal__fs__

// user's duty to free
// extra_byte: 0 or 1, if you were to read a string, we ensure another byte
// which is set '\0' to make a c-string.
unsigned char* s_read(const char* path, size_t* size, int extra_byte);
char* s_reads(const char* path);

size_t s_writes(const char* path, const char* string);

size_t s_writef(const char* path, const void* data, size_t size);

const char* s_get_write_path();


#endif /* defined(__zore__zore_fs__) */
