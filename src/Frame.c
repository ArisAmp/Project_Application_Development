#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include "util.h"
#include "ijvm.h"
#include "Stack.h"

typedef struct Frame{
    struct Frame * previous;
    word_t localVariables[10000];
    int framePC;

}Frame_t;

void initFrame(Frame_t *frame){
    frame->previous = NULL;
    frame->framePC = 0;
}





