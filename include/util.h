#ifndef UTIL_H
#define UTIL_H

/* debug print */
#ifdef DEBUG

#define dprintf(...) \
    fprintf(stderr,  __VA_ARGS__)
#else
#define dprintf(...)
#endif
uint32_t swap_uint32(uint32_t num){
    return ((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000);
}

#endif
