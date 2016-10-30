#include "platform.h"

#ifdef PLAT_WIN32

int gettimeofday(struct timeval * val, struct timezone *z)
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

int strcasecmp(char *s1, char *s2)
{
    while (toupper((unsigned char)*s1) == toupper((unsigned char)*s2++))
        if (*s1++ == '\0') return 0;
    return(toupper((unsigned char)*s1) - toupper((unsigned char)*--s2));
}

int strncasecmp(char *s1, char *s2, register int n)
{
    while (--n >= 0 && toupper((unsigned char)*s1) == toupper((unsigned char)*s2++))
        if (*s1++ == '\0')  return 0;
    return(n < 0 ? 0 : toupper((unsigned char)*s1) - toupper((unsigned char)*--s2));
}

#endif