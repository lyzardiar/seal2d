#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform/platform.h"
#include "platform/fs.h"

#include "memory.h"
#include "util.h"

unsigned char* s_read(const char* path, size_t* size, int extra_byte) {
#ifdef PLAT_DESKTOP
    s_assert(extra_byte == 0 || extra_byte == 1);
    FILE* fp = fopen(path, "r");
    if (!fp) {
        fprintf(stderr, "s_read, can't open file path = %s.\n", path);
        return NULL;
    }
    fseek(fp, 0L, SEEK_END);
    size_t file_size = ftell(fp);
    rewind(fp);
    
    unsigned char* buffer = (unsigned char*)s_malloc(file_size + extra_byte);
    memset(buffer, 0, file_size);
    size_t result = fread(buffer, 1, file_size, fp);
    
    if(result != file_size) {
        free(buffer);
        fclose(fp);
        fprintf(stderr, "s_read, file reading error, size not match?.\n");
        return NULL;
    }
    if(extra_byte) {
        buffer[file_size] = 0;
    }
    if (size) {
        *size = result;
    }
    fclose(fp);
    return buffer;
#endif
}

char* s_reads(const char* path) {
    return (char*)s_read(path, NULL, 1);
}

size_t s_writes(const char* path, const char* string) {
    size_t size = strlen(string)+1;
    return s_writef(path, string, size);
}

size_t s_writef(const char* path, const void* data, size_t size) {
#ifdef PLAT_DESKTOP
    FILE* fp = fopen(path, "w+");
    if (!fp) {
        fprintf(stderr, "s_writef, can't open file.\n");
        return 0;
    }
    if (!data) {
        fprintf(stderr, "s_writef, null data.\n");
        fclose(fp);
        return 0;
    }
    size_t write_size = fwrite(data, 1, size, fp);
    if (write_size != size) {
        fclose(fp);
        fprintf(stderr, "s_writef, error when write file. not enough disk?\n");
        return 0;
    }
    fclose(fp);
    return write_size;
#endif
}

#ifdef PLAT_MAC
extern const char* get_write_path_mac();
#endif

const char* s_get_write_path() {
#ifdef PLAT_MAC
    return get_write_path_mac();
#endif
	return "";
}