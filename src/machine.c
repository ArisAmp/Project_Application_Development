#include <ijvm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <util.h>
#include "BinaryLoader.h"
#include "Stack.h"


BinaryLoader_t *blt;
Stack_t *stack;
byte_t instruction;

int programCounter = 0;

int init_ijvm(char *binary_file)
{
    // Implement loading of binary here
    blt = (BinaryLoader_t *)malloc(sizeof(BinaryLoader_t));
    BinaryLoader(blt, binary_file);
    
    if (checkMagicBytes(blt) != 0){
        return -1;
    }
    getConstants(blt);
    getData(blt);
    
    return 0;
}

int text_size(){
    
    return blt->dataSize;
}

byte_t * get_text() {
    
    return blt->data;
}


void destroy_ijvm()
{
  // Reset IJVM state
    free(blt->buffer);
    free(blt->constants);
    free(blt->data);
    free(blt);
}

int get_program_counter()//how far in instructions
{
    return programCounter;
}

int tos()
{
    //Return top element of the stack
    return swap_uint32(getTop(stack));
    
}
    

word_t *get_stack()//byte_ or word_t because the stack has 10 (70)<-- this one inside and it is 2 bytes also change the bytes to hex.
{
    getStack(stack);
}

void executeBipush()
{
    word_t arg = blt->data[programCounter + 1];
    push(stack, arg);
    programCounter += 2;
}

void executeIadd()
{
    word_t arg1 = pop(stack);
    word_t arg2 = pop(stack);
    word_t res = interpretToInteger(arg1) + interpretToInteger(arg2);
    
    push(stack, res);
    programCounter +=1;
}

void executeIsub()
{
    word_t arg1 = pop(stack);
    word_t arg2 = pop(stack);
    word_t res = interpretToInteger(arg2) - interpretToInteger(arg1);
    
    push(stack, res);
    programCounter += 1;
}
void executeIand()
{
    word_t arg1 = pop(stack);
    word_t arg2 = pop(stack);
    word_t res = arg1 & arg2;
    
    push(stack, res);
    programCounter += 1;
}

void executeIor()
{
    word_t arg1 = pop(stack);
    word_t arg2 = pop(stack);
    word_t res = arg1 | arg2;
    
    push(stack, res);
    programCounter += 1;
}

void executeDup()
{
    word_t res = getTop(stack)
    push(stack, res);
    programCounter += 1;
}
    
void run()
{
    // Step while you can
    for (int i=0; i<blt->dataSize; i++){
        step();
    }
}

bool step(){
    instruction = blt->data[programCounter];
    
        switch (instruction)
            case OP_BIPUSH:
                executeBipush();
                break;
            case OP_DUP:
                executeDup();
                break;
            case OP_IADD:
                printf ("IADD\n");
                break;
            case OP_ISUB:
                printf ("ISUB\n");
                break;
            case OP_OUT:
                printf ("OUT\n");
                break;
            default:
                break;
        }
        return 0;
}

void set_input(FILE *fp)
{
  // TODO: implement me
}

void set_output(FILE *fp)
{
  // TODO: implement me
}
