#include "seal.h"

int main(int argc, char *argv[]) {
    seal_init();
    
    int running = 1;
    while (running) {
        float dt = 0.16;
        seal_update(dt);
        seal_draw();
    }
    
    seal_destroy();
    return 0;
}
