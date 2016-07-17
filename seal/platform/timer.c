#include "timer.h"

#ifdef WIN32 
#include <winsock.h>
#include <windows.h>
#else
#include <sys/time.h>
#endif
#ifdef WIN32
int gettimeofday(struct timeval * val, struct timezone *tz)
{
	if (val)
	{
		LARGE_INTEGER liTime, liFreq;
		QueryPerformanceFrequency(&liFreq);
		QueryPerformanceCounter(&liTime);
		val->tv_sec = (long)(liTime.QuadPart / liFreq.QuadPart);
		val->tv_usec = (long)(liTime.QuadPart * 1000000.0 / liFreq.QuadPart - val->tv_sec * 1000000.0);
	}
	return 0;
}
#endif

long gettime() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec)*1000 + time.tv_usec*0.001;
}