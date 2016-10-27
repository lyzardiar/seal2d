#include <stdio.h>
#include <string.h>

#include "hashmap.h"

static int hash_str(void* key)
{
    return hashmapHash(key, strlen(key));
}

static bool hash_equal(void* a, void* b)
{
    return strcmp(a, b) == 0;
}

void test_hash_map()
{
    struct Hashmap* map = hashmapCreate(2, hash_str, hash_equal);
    const char* key = "test_key";
    const char* data = "test_data";

    hashmapPut(map, (void*)key, (void*)data);

    const char* d = hashmapGet(map, (void*)key);
    printf("d = %s\n", d);
}