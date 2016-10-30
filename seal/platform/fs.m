#include "platform/platform.h"

#ifdef __APPLE__
#import <Foundation/Foundation.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform/fs.h"

#include "memory.h"
#include "util.h"

const char* fs_full_path(const char* filename) {
    char* ext_offset = strrchr(filename, '.');
    if (!ext_offset) {
        fprintf(stderr, "invalid file name.\n");
        return NULL;
    }
    
    long name_len = ext_offset - filename;
    
    
    char name[name_len+1];
    memset(name, 0, name_len+1);
    strncpy(name, filename, name_len);

    NSString *dataFile = [[NSBundle mainBundle] pathForResource:
                          [NSString stringWithUTF8String:name] ofType:
                          [NSString stringWithUTF8String:ext_offset+1]]; // +1 to escape the 'dot'.

    return [dataFile UTF8String];
}


const char* fs_sandbox_root_path() {
    #ifdef PLAT_IOS
        return [[[NSBundle mainBundle] resourcePath] UTF8String];
    #endif

    #ifdef PLAT_DESKTOP
        return "";
    #endif
}

unsigned char* fs_read(const char* path, size_t* size, int extra_byte) {
#ifdef PLAT_IOS
    const char* full_path = fs_full_path(path);
#endif
    
#ifdef PLAT_DESKTOP
    const char* full_path = path;
#endif
    
    s_assert(extra_byte == 0 || extra_byte == 1);
    FILE* fp = fopen(full_path, "r");
    if (!fp) {
        fprintf(stderr, "s_read, can't open file path = %s.\n", full_path);
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
}

char* fs_reads(const char* path) {
    return (char*)fs_read(path, NULL, 1);
}

size_t fs_writes(const char* path, const char* string) {
    size_t size = strlen(string)+1;
    return fs_writef(path, string, size);
}

size_t fs_writef(const char* path, const void* data, size_t size) {
    FILE* fp = fopen(path, "w+");
    if (!fp) {
        fprintf(stderr, "fs_writef, can't open file.\n");
        return 0;
    }
    if (!data) {
        fprintf(stderr, "fs_writef, null data.\n");
        fclose(fp);
        return 0;
    }
    size_t write_size = fwrite(data, 1, size, fp);
    if (write_size != size) {
        fclose(fp);
        fprintf(stderr, "fs_writef, error when write file. not enough disk?\n");
        return 0;
    }
    fclose(fp);
    return write_size;
}

static const char* write_path = NULL;
const char* fs_get_write_path() {
    if (!write_path) {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths objectAtIndex:0];
        write_path = [documentsDirectory UTF8String];
    }
    return write_path;
}
