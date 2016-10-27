#ifndef __seal__sprite_util__
#define __seal__sprite_util__

#include <assert.h>

#define stringfy_bool(v) (v ? "true" : "false")

#ifdef DEBUG
#define s_assert(e) ((void)((e)|| \
(fprintf(stderr, "%s:%d: Assertion failed: %s\n", \
__FILE__, (int)__LINE__, #e), abort(), 0)))
#else
#define s_assert(e) (0)
#endif

#endif
