#include "timer.h"

#ifdef WIN32
#else
#include <sys/time.h>
#endif
long gettime() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec)*1000 + time.tv_usec*0.001;
}