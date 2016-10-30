#include "platform/platform.h"
#include "fs.h"

#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>

const char* fs_full_path(const char* filename)
{

}

const char* fs_sandbox_root_path()
{

}

unsigned char* fs_read(const char* path, size_t* size, int extra_byte)
{
    FILE* fp = fopen(path, "rb");
    assert(fp);
    int descriptor = _fileno(fp);
    struct stat stat_buf;
    if (fstat(descriptor, &stat_buf) == -1) {
        fclose(fp);
        assert(0);
    }
    size_t len = stat_buf.st_size;
    char* buff = malloc(len + 1);
    size_t rs = fread(buff, 1, len, fp);
    buff[len] = 0;
    fclose(fp);

    if (size != NULL) {
        *size = rs;
    }

    return buff;
}

char* fs_reads(const char* path)
{
    return (char*)fs_read(path, NULL, 0);
}

size_t fs_writes(const char* path, const char* string)
{

}

size_t fs_writef(const char* path, const void* data, size_t size)
{

}

const char* fs_get_write_path()
{
    return "";
}