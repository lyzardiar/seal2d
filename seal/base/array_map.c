#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "array_map.h"

#define DEFAULT_SIZE (32)

struct array_map* array_map_new() {
    struct array_map* map = (struct array_map*)malloc(sizeof(struct array_map));
    map->cur_id = 0;
    map->capacity = DEFAULT_SIZE;
    map->entries = (struct handle_entry*)calloc(DEFAULT_SIZE, sizeof(struct handle_entry));
    return map;
}

void array_map_delete(struct array_map* map) {
    if (map) {
        free(map->entries);
        free(map);
    }
}

handle_id array_map_add(struct array_map* map, void* user_data) {
    if(!user_data) {
        return 0;
    }
    
    int cap = map->capacity;
    unsigned int id = map->cur_id;
    
    ++id;
    
    if(id > cap * 3/4) {
        struct handle_entry* new_entries = (struct handle_entry*)calloc(cap*2, sizeof(struct handle_entry));
        for (int i = 0; i < map->capacity; ++i) {
            new_entries[i] = map->entries[i];
        }
        free(map->entries);
        map->entries = new_entries;
        map->capacity = cap*2;
    }
    
    // a&(n-1) <==> a%n when n is power of 2
    id = id & (map->capacity-1);
    if (id == 0) {
        id = 1;
    }
    
    map->cur_id = id;
    struct handle_entry* e = &map->entries[id];
    e->hid = id;
    e->ref = 1;
    e->user_data = user_data;
    return id;
}

void* array_map_get(struct array_map* map, handle_id id) {
    if(id == 0 || id > map->capacity) {
        return 0;
    }
    map->entries[id].ref++;
    return map->entries[id].user_data;
}

void array_map_put(struct array_map* map, handle_id id) {
    struct handle_entry* entry = &map->entries[id];
    entry->ref--;
}

void array_map_foreach(struct array_map* map, array_map_iter iter) {
    if(!iter) {
        return;
    }
    
    for (int i = 1; i <= map->cur_id; ++i) {
        iter(&map->entries[i]);
    }
}

void array_map_debug_visit(struct handle_entry* entry) {
    printf("entry = { hid = %d, ref = %d, user_data = %p}\n",
           entry->hid, entry->ref, entry->user_data);
}

void array_map_debug_dump(struct array_map* map) {
    array_map_foreach(map, array_map_debug_visit);
}
