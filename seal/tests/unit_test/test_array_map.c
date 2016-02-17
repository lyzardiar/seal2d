#include "seal.h"

#include "test.h"

#include "base/array_map.h"

void test_array_map() {
    struct array_map* map = array_map_new();
    unsigned long data[] = { 0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0x0a};
    
    printf("lenof(data) = %ld\n", sizeof(data)/sizeof(unsigned long));
    for (int i = 0; i < sizeof(data)/sizeof(unsigned long); ++i) {
        array_map_add(map, (void*)data[i]);
    }
    
    assert(map->cur_id == 10);
    
    for (int i = 0; i < sizeof(data)/sizeof(unsigned long); ++i) {
        array_map_add(map, (void*)data[i]);
    }
    array_map_debug_dump(map);

    for (int i = 0; i < sizeof(data)/sizeof(unsigned long); ++i) {
        array_map_add(map, (void*)data[i]);
    }
    
    for (int i = 0; i < sizeof(data)/sizeof(unsigned long); ++i) {
        array_map_add(map, (void*)data[i]);
    }
    
    array_map_debug_dump(map);
    assert(map->cur_id == 40);
    
    assert((unsigned long)array_map_get(map, 12) == 0x2);
    assert((unsigned long)array_map_get(map, 22) == 0x2);
    assert((unsigned long)array_map_get(map, 32) == 0x2);
}