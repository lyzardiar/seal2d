#include "memory.h"
#include "array.h"

#include "bmfont.h"

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


//example data:

//info face=".AppleSystemUIFont" size=64 bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1 padding=0,0,0,0 spacing=1,1
//common lineHeight=76 base=62 scaleW=512 scaleH=512 pages=1 packed=0
//page id=0 file="animated.png"
//chars count=97
//char id=32     x=125   y=472   width=0     height=0     xoffset=0     yoffset=66    xadvance=16    page=0 chnl=0 letter="space"
//char id=33     x=404   y=316   width=20    height=58    xoffset=3     yoffset=8     xadvance=17    page=0 chnl=0 letter="!"

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
        
        printf("line = %s\n", line);
        printf("tag = %s\n", tag);
        if (!strcmp(tag, "info")) {
            sscanf(line, "%s %s %s %s %s %s %s %s %s %s %s %s",
                   tag, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, dummy, space);
            
            printf("space = %s\n", space);
            char space_data[16] = "";
            parse_str(space, space_data);
            
            printf("space_data = %s\n", space_data);
            
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
            
            printf("lineHeight = %d, base = %d, scaleW = %d, scaleH = %d, pages = %d, packed = %d\n",
                   font->common.lineHeight, font->common.base, font->common.scaleW,
                   font->common.scaleH, font->common.pages, font->common.packed);

        } else if (!strcmp(tag, "page")) {
            char page[32] = "";
            char file[128] = "";
            sscanf(line, "%s %s %s", tag, page, file);
            font->page.id = parse_int(page);
            parse_str(file, font->page.file);
            
            printf("page.id = %d, file = %s\n", font->page.id, font->page.file);
        } else if (!strcmp(tag, "chars")) {
            char count[16] = "";
            sscanf(line, "%s %s", tag, count);
            int char_count = parse_int(count);
            printf("char_count = %d\n", char_count);
            
            struct array* characters = array_new(char_count);
        
            // example data struct:
            // char id=38     x=359   y=73    width=53    height=59    xoffset=1     yoffset=8     xadvance=43    page=0 chnl=0 letter="&"
            
            char dummy[32] = "";
            char id[32] = "";
            char x[32] = "";
            char y[32] = "";
            char width[32] = "";
            char height[32] = "";
            char xoffset[32] = "";
            char yoffset[32] = "";
            char xadvance[32] = "";
            char page[32] = "";
            char chnl[32] = "";
            char letter[32] = "";
            
            // this code is stupid. will fix this someday :)
            for (int i = 0; i < char_count; ++i) {
                line = strtok(NULL, "\n");
                struct charc* c = STRUCT_NEW(charc);
                sscanf(line, "%s %s %s %s %s %s %s %s %s %s %s %s",
                       dummy, id, x, y, width, height, xoffset, yoffset, xadvance, page, chnl, letter);
                c->id = parse_int(id);
                c->x = parse_int(x);
                c->y = parse_int(y);
                c->width = parse_int(width);
                c->height = parse_int(height);
                c->xoffset = parse_int(xoffset);
                c->yoffset = parse_int(yoffset);
                c->xadvance = parse_int(xadvance);
                c->page = parse_int(page);
                c->chnl = parse_int(chnl);
                parse_str(letter, c->letter);
                printf("id=%d, x=%d, y=%d, width=%d, height=%d, xoffset=%d, yoffset=%d, xadvance=%d, page=%d, chnl=%d, letter=%s\n",
                       c->id, c->x, c->y, c->width, c->height,
                       c->xoffset, c->yoffset, c->xadvance, c->page, c->chnl, c->letter);
                array_push_back(characters, c);
            }
            
            font->characters = characters;
            break; //finished reading
        }
        
        line = strtok(NULL, "\n");
    }
    return font;
}

void bmfont_free(struct bmfont* self);