#include "memory.h"

struct alloc_entry {
    void *p;
    unsigned long size;
    const char* file;
    int line;
    struct list_head list;
};

static struct list_head* head = NULL;

void add_entry(void* ptr, size_t size, const char* file, int line) {
    struct alloc_entry* entry = (struct alloc_entry*)malloc(sizeof(struct alloc_entry));
    
    entry->file = file;
    entry->line = line;
    entry->size = size;
    INIT_LIST_HEAD(&entry->list);
    
    if (!head) {
        static struct list_head l_head;
        INIT_LIST_HEAD(&l_head);
        head = &l_head;
        list_add(&entry->list, &l_head);
    } else {
        list_add_tail(&entry->list, head);
    }
    
    entry->p = ptr;
}

void delete_entry(void* ptr) {
    struct alloc_entry* iter = NULL;
    struct alloc_entry* found = NULL;
    
    list_for_each_entry(iter, struct alloc_entry, head, list) {
        if (iter->p == ptr) {
            found = iter;
            break;
        }
    }
    
    list_del(&found->list);
    free(found);
}

void* seal_malloc(size_t size, const char* file, int line) {
    
    void* p = (void*)malloc(size);
    add_entry(p, size, file, line);
    return p;
}

void* seal_calloc(size_t size, const char* file, int line) {
    void* p = seal_malloc(size, file, line);
    memset(p, 0, size);
    return p;
}

void* seal_realloc(void* ptr, size_t size, const char* file, int line) {
    void* p = realloc(ptr, size);
    if (p != ptr) {
        delete_entry(ptr);
        add_entry(p, size, file, line);
    }
    return p;
}

void seal_free(void* ptr) {
    delete_entry(ptr);
    free(ptr);
}

void seal_dump_memory() {
    unsigned long total = 0;
    
    struct alloc_entry* iter = NULL;
    
    list_for_each_entry(iter, struct alloc_entry, head, list) {
        total = total + iter->size;
        printf("Alloc file = [%s], line = [%d], pointer = [%p] \n", iter->file, iter->line, iter->p);
    }
    
    printf("total allocated memory in [%ld] bytes, [%.2f] MB \n", total, ((float)total)/(1024*1024));
}