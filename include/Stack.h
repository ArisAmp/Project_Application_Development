#include "ijvm.h"
#ifndef HEADER_STACK
#define HEADER_STACK

typedef struct Stack {

    int top;
    word_t* array;
    unsigned int size;

}Stack_t;

void createStack(int size);
void isFull (Stack_t* );
void isEmpty (Stack_t* );
void push (Stack_t* , word_t );
word_t pop (Stack_t* );
word_t getTop (Stack_t* );
int getSize (Stack_t* );
word_t * getStack();

#endif
