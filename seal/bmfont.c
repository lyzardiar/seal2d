#include "memory.h"
#include "hashmap.h"

#include "sprite.h"
#include "bmfont.h"

static int64_t parse_int64(char* data) {
    int64_t n = 0;
    char* pos = strchr(data, '='); // go through the key
    
    sscanf(pos+1, "%lld", &n);
    return n;
    
}

static int parse_int(char* data) {
    int n = 0;
    char* pos = strchr(data, '='); // go through the key
    
    sscanf(pos+1, "%d", &n);
    return n;
}

static void parse_str(char* data, char* val) {
    char* pos = strchr(data, '=');
    sscanf(pos+1, "%s", val);
}

static int hash_str(void* key) {
    return hashmapHash(key, strlen(key));
}

static bool hash_equal(void* a, void* b) {
    return strcmp(a, b) == 0;
}


//example data:

//info face=".AppleSystemUIFont" size=64 bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1 padding=0,0,0,0 spacing=1,1
//common lineHeight=76 base=62 scaleW=512 scaleH=512 pages=1 packed=0
//page id=0 file="animated.png"
//chars count=97
//char id=32     x=125   y=472   width=0     height=0     xoffset=0     yoffset=66    xadvance=16    page=0 chnl=0 letter="space"
//char id=33     x=404   y=316   width=20    height=58    xoffset=3     yoffset=8     xadvance=17    page=0 chnl=0 letter="!"

static void remove_quote(char* dst, const char* src) {
    memcpy(dst, src+1, strlen(src)-2);
    dst[strlen(src)-2] = 0;
}

struct bmfont* bmfont_new(const char* bmfont_data) {
    struct bmfont* font = STRUCT_NEW(bmfont);
    memset(font, 0, sizeof(struct bmfont));
    
    char dummy[256] = "";
    char space[64] = "";
    
    char* line = strtok((char*)bmfont_data, "\n");
    
    char tag[32] = "";
    
    while(line) {
        memset(tag, 0, 32);
        sscanf(line, "%s", tag);
        
        if (!strcmp(tag, "info")) {
            sscanf(line, "%s %s %s %s %s %s %s %s %s %s %s %s",
                   tag, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, space);
            
            char space_data[16] = "";
            parse_str(space, space_data);
            sscanf(space_data, "%d,%d", &font->info.spacing.x, &font->info.spacing.y);
        } else if (!strcmp(tag, "common")) {
            
            char lineHeight[32] = "";
            char base[32] = "";
            char scaleW[32] = "";
            char scaleH[32] = "";
            char pages[32] = "";
            char packed[32] = "";
            
            sscanf(line, "%s %s %s %s %s %s %s", tag, lineHeight, base, scaleW, scaleH, pages, packed);
            
            font->common.lineHeight = parse_int(lineHeight);
            font->common.base = parse_int(base);
            font->common.scaleW = parse_int(scaleW);
            font->common.scaleH = parse_int(scaleH);
            font->common.pages = parse_int(pages);
            font->common.packed = parse_int(packed);
        } else if (!strcmp(tag, "page")) {
            char page[32] = "";
            char file[128] = "";
            sscanf(line, "%s %s %s", tag, page, file);
            font->page.id = parse_int(page);
            
            char tmp[128] = "";
            parse_str(file, tmp);
            remove_quote(font->page.file, tmp);
        } else if (!strcmp(tag, "chars")) {
            char count[16] = "";
            sscanf(line, "%s %s", tag, count);
            int char_count = parse_int(count);
            
            struct Hashmap* characters = hashmapCreate(256, hash_str, hash_equal);
            
            char dummy[32] = "";
            
            // this code is stupid. will fix this someday :)
            for (int i = 0; i < char_count; ++i) {
                line = strtok(NULL, "\n");
                struct charc* c = STRUCT_NEW(charc);
                
                sscanf(line, "char%[ ]id=%lld%[ ]x=%d%[ ]y=%d%[ ]width=%d%[ ]height=%d%[ ]xoffset=%d%[ ]yoffset=%d%[ ]xadvance=%d%[ ]page=%d%[ ]chnl=%d%[ ]letter=\"%[^\n]"
                       , dummy , &c->id
                       , dummy , &c->x         , dummy , &c->y
                       , dummy , &c->width     , dummy , &c->height
                       , dummy , &c->xoffset   , dummy , &c->yoffset
                       , dummy , &c->xadvance  , dummy , &c->page
                       , dummy , &c->chnl      , dummy , c->letter
                       );
                
                // TODO: ugly code, improve someday.
                
                if (c->id == 32) { // space for special case
                    c->letter[0] = ' ';
                    c->letter[1] = 0;
                } else {
                    c->letter[strlen(c->letter) - 1] = 0;
                }
                
                hashmapPut(characters, c->letter, c);
            }
            
            font->characters = characters;
            break; //finished reading
        }
        
        line = strtok(NULL, "\n");
    }
    return font;
}

void bmfont_free(struct bmfont* self) {
    hashmapFree(self->characters);
    
    s_free(self);
}

// add a len param we may avoid a lot string copy.
struct charc* bmfont_load_charc(struct bmfont* self, const char* c) {
    // TODO: implement a function which convert utf-8 to integer.
    // NOW we simply return A
    
    return hashmapGet(self->characters, (void*)c);
}
