/*
 * Created by JonathanOsterman on 13-6-17.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include "util.h"
#include "ijvm.h"


//A structure to represent the stack
typedef struct Stack {
    int top;
    word_t* array;
    unsigned int size;
}Stack_t;

//Function that initializes size of stack
void createStack(Stack_t *stack, unsigned int size){

    word_t *contents;
    contents = (word_t*)malloc(sizeof(word_t) * stack->size);

    if (contents == NULL) {
        fprintf(stderr, "Insufficient memory to initialize stack.\n");
        exit(1);  // Exit, returning error code. 
    }

    stack->array = contents;
    stack->size = size;
    stack->top = -1;
}

//Check if stack is full when the top is equal with the last index
int isFull(Stack_t *stack){

    if (stack->top == stack->size - 1){
        return 1;
    }
    return 0;
}

//Check if stack is empty when the top of the stack is equal to -1
int isEmpty(Stack_t *stack){

    if (stack->top == -1){
        return 1;
    }
    return 0;
}

// Implement push function, increases top by 1
// element: BIG ENDIAN
void push(Stack_t *stack, word_t element){

    if (isFull(stack) == 1){
        fprintf (stderr, "Stack Overflow");
        return;
    }
    stack->top++;
    stack->array[stack->top] = element;

//    printf("%x pushed to stack\n", element);
}

//Implement pop function, decreases top by 1
// BIG ENDIAN
word_t pop(Stack_t *stack){
    word_t result;

    if (isEmpty(stack) == 1){
        fprintf(stderr, "Stack Underflow");
        return -1;
    }
    result = stack->array[stack->top];
    stack->top--;
    return result;
}

//Return the top element of the array without removing it
word_t getTop(Stack_t *stack){

    return stack->array[stack->top];
}

//Return the size of the stack
int getSize(Stack_t *stack){
    return stack->size;
}


void stackDestroy(Stack_t *stack)
{
    free(stack->array);

    stack->array = NULL;
    stack->size = 0;
    stack->top = -1;  /* I.e., empty */
}



/*
int main(){


    Stack_t fstack;

    createStack(&fstack, 100);

    push(&fstack, 0x10);
    push(&fstack, 0xff);
    push(&fstack, 0x30);

    printf("%x popped from stack\n", pop(&fstack));

    printf("%d\n",swap_uint32(getTop(&fstack)));
    printf("%d\n",getSize(&fstack));

    getStack(&fstack);

    stackDestroy(&fstack);
}
*/
