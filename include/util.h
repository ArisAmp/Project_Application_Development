#ifndef UTIL_H
#define UTIL_H
#include "ijvm.h"

/* debug print */
#ifdef DEBUG

#define dprintf(...) \
    fprintf(stderr,  __VA_ARGS__)
#else
#define dprintf(...)
#endif

word_t swap_uint32(word_t);
#endif
