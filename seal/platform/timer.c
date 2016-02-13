#include "timer.h"

long gettime() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec)*1000 + time.tv_usec*0.001;
}