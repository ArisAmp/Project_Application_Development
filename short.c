#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "util.h"


typedef int8_t byte_t;

int shortToSignedInt(byte_t arg1, byte_t arg2)//try this
{
    int result = ((arg1 << 8) | (arg2 & 0xff));
    return result;
}


int shortToUnsignedInt(byte_t arg1, byte_t arg2)
{
    int result = (((arg1 & 0xFF) << 8)|(arg2 & 0xFF));
    return result;
}




int main(){
    byte_t byte1 = "A";
    byte_t byte2 = "B";
    
    printf("%d",shortToSignedInt(byte1, byte2));

    
}

