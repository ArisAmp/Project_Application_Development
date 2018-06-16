#ifndef HEADER_STACK
#define HEADER_STACK

typedef struct Stack {
    
    int top;
    word_t* array;
    unsigned int size;
    
}Stack_t;

void Stack_t* createStack(size);
void isFull (Stack_t* stack);
void isEmpty (Stack_t* stack);
void push (Stack_t* stack, word_t element);
word_t pop (Stack_t* stack);
word_t getTop (Stack_t* stack);
int getSize (Stack_t* stack);
int hexadecimalToDecimal(word_t x);

#endif
