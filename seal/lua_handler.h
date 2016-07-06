#ifndef lua_handler_h
#define lua_handler_h

struct Hashmap;

#define LUA_FUNCTION_HANDLER_KEY "LUA_FUNCTION_HANDLER_KEY"

struct lua_handler {
    struct Hashmap* __handlers;
};

typedef long LUA_FUNCTION_HANDLER;

struct lua_handler* lua_handler_new();
void lua_handler_free(struct lua_handler* self);

void lua_handler_set(struct lua_handler* self, void* object, LUA_FUNCTION_HANDLER lua_function);

void lua_handler_clean(struct lua_handler* self, void* object);


#endif
