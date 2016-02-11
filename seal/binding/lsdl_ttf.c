#include "seal.h"

extern struct game *GAME;

typedef struct font_desc {
    char path[256];
    TTF_Font* font;
    int font_size;
    int font_index;
    SDL_Color text_color;
}font_desc;

#define MAX_FONT 16
static font_desc* font_opend[MAX_FONT] = {NULL};
static int font_index = 0;

int lsdl_ttf_free(lua_State* L) {
    int i = 0;
    for(i = 0; i < MAX_FONT; ++i) {
        if(font_opend[i] != NULL) {
            SDL_free(font_opend[i]);
        }
    }
    return 0;
}

int lsdl_ttf_openfont(lua_State* L) {
    size_t len = 0;
    const char* font_path = luaL_checklstring(L, 1, &len);
    lua_Integer font_size = luaL_checkinteger(L, 2);

    // search from the cache first
    int i;
    for(i = 0; i < MAX_FONT; ++i) {
        font_desc* fd = font_opend[i];
        if (fd && strcmp(fd->path, font_path) == 0 && fd->font_size == font_size) {
            lua_pushlightuserdata(L, fd);
            return 1;
        }
    }
    
    TTF_Font* font = TTF_OpenFont(font_path, (int)font_size);
    
    SDL_assert(font);
    
    font_desc* fd = (font_desc*)SDL_malloc(sizeof(font_desc));
    font_opend[font_index] = fd;

    fd->font = font;
    fd->font_size = (int)font_size;
    fd->font_index = font_index++;
    fd->text_color.r = 255;
    fd->text_color.g = 0;
    fd->text_color.b = 0;
    fd->text_color.a = 0;
    strcpy(fd->path, font_path);
    
    lua_pushlightuserdata(L, fd);
    return 1;
}

int lsdl_ttf_load_texture(lua_State* L) {
    font_desc* fd = lua_touserdata(L, 1);
    const char* text = luaL_checkstring(L, 2);
    
    SDL_Surface* surface = TTF_RenderText_Solid(fd->font, text, fd->text_color);
    if (surface) {
        int width = surface->w;
        int height = surface->h;
        SDL_Texture* texture = SDL_CreateTextureFromSurface(GAME->gl_render, surface);
        SDL_FreeSurface(surface);
        if(!texture) {
            return luaL_error(L, "sdlttf.load texture %s failed.", fd->path);
        }
        lua_pushlightuserdata(L, texture);
        lua_pushinteger(L, width);
        lua_pushinteger(L, height);
        return 3;
    } else {
        return luaL_error(L, "sdlttf.load surface %s failed.", fd->path);
    }
}

int luaopen_sdl_ttf_core(lua_State* L) {
    
#ifdef luaL_checkversion
    luaL_checkversion(L);
#endif
    luaL_Reg lib[] = {
        { "open", lsdl_ttf_openfont},
        { "load_texture", lsdl_ttf_load_texture},
        { NULL, NULL },
    };
    
    luaL_newlib(L, lib);
    
    return 1;
}