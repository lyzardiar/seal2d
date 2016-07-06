#include "memory.h"
#include "util.h"

#include "hashmap.h"

#include "lua_handler.h"

static int hash_str(void* key) {
    return hashmapHash(key, sizeof(void*));
}

static bool hash_equal(void* a, void* b) {
    return a == b;
}

struct lua_handler* lua_handler_new() {
    struct lua_handler* handler = STRUCT_NEW(lua_handler);
    handler->__handlers = hashmapCreate(128, hash_str, hash_equal);
    return handler;
}

void lua_handler_free(struct lua_handler* self) {
    hashmapFree(self->__handlers);
    s_free(self);;
}

void lua_handler_set(struct lua_handler* self, void* object, LUA_FUNCTION_HANDLER lua_function) {
    s_assert(object);
    hashmapPut(self->__handlers, object, (void*)lua_function);
}

void lua_handler_clean(struct lua_handler* self, void* object) {
    s_assert(object);
    hashmapRemove(self->__handlers, object);
}