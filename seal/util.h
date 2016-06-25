#ifndef __seal__sprite_util__
#define __seal__sprite_util__

#include <assert.h>

#define stringfy_bool(v) (v ? "true" : "false")

#ifdef DEBUG
// others
#define s_assert(condition)     assert(condition)

#else

#define s_assert(condition)

#endif

#endif
