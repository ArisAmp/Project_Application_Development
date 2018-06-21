#include "ijvm.h"
#include "Stack.h"
#ifndef HEADER_FRAME
#define HEADER_FRAME

typedef struct Frame{
    struct Frame * previous;
    word_t localVariables[10000];
    int framePC;
    
}Frame_t;

void initFrame(Frame_t *frame);

#endif
