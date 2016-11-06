
#include "jni.h"
#include "android/asset_manager.h"
#include "android/asset_manager_jni.h"

#include "fs.h"

AAssetManager* fs_assetmanager = NULL;

char writable_path[256] = {0};
const int relative_path_len = 3;

char* relative_path[] = {
        "",
        "../../",
        "../../../../luasrc/"
};

int fs_exists(const char* filename)
{
    if (filename == NULL) return 0;

    int found = 1;

    if (filename[0] != '/') {
        if (fs_assetmanager) {
            AAsset* aa = AAssetManager_open(fs_assetmanager, filename, AASSET_MODE_UNKNOWN);
            if (aa) {
                AAsset_close(aa);
                found = 0;
            }
        }
    } else {
        FILE* fp = fopen(filename, "r");
        if (fp != NULL) {
            fclose(fp);
            found = 0;
        }
    }
    return found;
}

const char* fs_full_path(const char* filename)
{
    static char* full_path[256] = { 0 };

    // todo: should cache full path
    int i = 0;
    const char* wp = fs_get_write_path();

    // first check storage
    for (i = 0; i < relative_path_len; ++i) {
        memset(full_path, 0, 256);
        strcat(full_path, wp);
        strcat(full_path, relative_path[i]);
        strcat(full_path, filename);

        if (fs_exists(full_path) == 0) {
            return full_path;
        }
    }

    // check apk
    if (fs_exists(filename) == 0) {
        return filename;
    }

    return filename;
}

const char* fs_sandbox_root_path()
{
    return fs_get_write_path();
}

unsigned char* fs_read(const char* path, size_t* osize, int extra_byte)
{
    const char* full_path = fs_full_path(path);
    char* buff = NULL;

    if (fs_exists(full_path) != 0){
        return NULL;
    }

    // in apk
    if (strcmp(full_path, path) == 0) {
        AAsset* asset = AAssetManager_open(fs_assetmanager, full_path, AASSET_MODE_UNKNOWN);
        off_t size = 0;
        int readsize = 0;

        if (NULL == asset) {
            return NULL;
        }

        size = AAsset_getLength(asset);
        buff = seal_malloc(size + 1, __FILE__, __LINE__);

        readsize = AAsset_read(asset, buff, size);
        AAsset_close(asset);

        if (readsize < size) {
            seal_free(buff);
            readsize = 0;
        }

        if (osize != NULL) {
            *osize = readsize;
        }
    } else { // raw file
        FILE* fp = fopen(full_path, "rb");
        assert(fp);
        int descriptor = fileno(fp);
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

        if (osize != NULL) {
            *osize = rs;
        }
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
    return writable_path;
}

void fs_set_write_path(const char* path)
{
    int len = strlen(path);
    strcpy(writable_path, path);

    if (writable_path[len - 1] != '/'){
        strcat(writable_path, "/");
    }
}

