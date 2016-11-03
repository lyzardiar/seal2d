/*
 * Copyright (C) 2016 Tang Yiyang
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See BELOW for details.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "../seal.h"

char writable_path[256] = {0};
const int relative_path_len = 3;

char* relative_path[] = {
    "",
    "../../",
    "../../../../luasrc/"
};

int fs_exists(const char* filename)
{
    FILE* fp = fopen(filename, "r");
    if (fp != NULL) {
        fclose(fp);
        return 0;
    }
    return 1;
}

const char* fs_full_path(const char* filename)
{
    static char* full_path[256] = { 0 };

    // todo: should cache full path
    int i = 0;
    const char* wp = fs_get_write_path();

    for (i = 0; i < relative_path_len; ++i) {
        full_path[0] = 0;
        strcat(full_path, wp);
        strcat(full_path, relative_path[i]);
        strcat(full_path, filename);

        if (fs_exists(full_path) == 0) {
            return full_path;
        }
    }
    return filename;
}

const char* fs_sandbox_root_path()
{
    return fs_get_write_path();
}

unsigned char* fs_read(const char* path, size_t* size, int extra_byte)
{
    const char* full_path = fs_full_path(path);

    FILE* fp = fopen(full_path, "rb");
    assert(fp);
    int descriptor = _fileno(fp);
    struct stat stat_buf;
    if (fstat(descriptor, &stat_buf) == -1) {
        fclose(fp);
        assert(0);
    }
    size_t len = stat_buf.st_size;
    char* buff = seal_malloc(len + 1, __FILE__, __LINE__);
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
    return fs_writef(path, string, strlen(string));
}

size_t fs_writef(const char* path, const void* data, size_t size)
{
    const char* full_path = fs_full_path(path);

    FILE* fp = fopen(full_path, "wb+");
    assert(fp);
    fwrite(data, size, 1, fp);
    fclose(fp);
}

const char* fs_get_write_path()
{
    if (writable_path[0] == '\0') {
        char exeFullPath[256];
        GetModuleFileName(NULL, exeFullPath, MAX_PATH);
        strcpy(writable_path, exeFullPath);

        char* ptr = &writable_path[strlen(writable_path) - 1];
        while (*ptr != '\0' && *ptr != '\\') ptr--;
        *ptr++ = '\\';
        *ptr = '\0';
    }

    return writable_path;
}