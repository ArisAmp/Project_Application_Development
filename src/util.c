#include "util.h"

word_t swap_uint32(word_t num){
    return ((num>>24)&0xff) | ((num<<8)&0xff0000) | ((num>>8)&0xff00) | ((num<<24)&0xff000000);
}

int interpret_to_integer(byte_t num){
    int res = (int)swap_uint32(num);
}
