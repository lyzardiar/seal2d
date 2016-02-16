#ifndef __handle_map__
#define __handle_map__

// inspired by this blog post, but we don't need thread safe here.
// http://blog.codingnow.com/2015/04/handlemap.html

struct handle_entry {
    void* user_data;
    unsigned int hid;
    unsigned int ref;
};

typedef unsigned int handle_id;
typedef void (*array_map_iter)(struct handle_entry*);

struct array_map {
    unsigned int cur_id;
    unsigned int capacity;
    struct handle_entry* entries;
};

struct array_map* hm_new();
void hm_delete(struct array_map* map);

handle_id hm_add(struct array_map* map, void* user_data);
void* hm_get(struct array_map* map, handle_id id);
void hm_put(struct array_map* map, handle_id id);
void hm_foreach(struct array_map* map, array_map_iter iter);

void hm_debug_dump(struct array_map* map);


#endif
