#include <ijvm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "util.h"
#include "BinaryLoader.h"
#include "Stack.h"


BinaryLoader_t *blt;
Stack_t *stack;
byte_t instruction;

int programCounter = 0;

FILE * output_file;
FILE * input_file;

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

int tos()//Return top element of the stack
{

    return swap_uint32(getTop(stack));

}

// TODO: add frame pointer
word_t * get_stack(){
  return stack->array;
}


void executeBipush()
{
    word_t arg = blt->data[programCounter + 1];
    push(stack, swap_uint32(arg));
    programCounter += 2;
}

void executeIadd()//pop as big endian execute the add as little endian and push back in big endian
{
    word_t arg1 = pop(stack);
    word_t arg2 = pop(stack);
    word_t res = swap_uint32(arg1) + swap_uint32(arg2);

    push(stack,swap_uint32(res));
    programCounter +=1;
}

void executeIsub()
{
    word_t arg1 = pop(stack);
    word_t arg2 = pop(stack);
    word_t res = swap_uint32(arg2) - swap_uint32(arg1);

    push(stack, swap_uint32(res));
    programCounter += 1;
}
void executeIand()
{
    word_t arg1 = swap_uint32(pop(stack));//swap to little endian in order to and both args and then swap to push back in big endian
    word_t arg2 = swap_uint32(pop(stack));
    word_t res = arg1 & arg2;

    push(stack, swap_uint32(res));
    programCounter += 1;
}

void executeIor()
{
    word_t arg1 = swap_uint32(pop(stack));
    word_t arg2 = swap_uint32(pop(stack));
    word_t res = arg1 | arg2;

    push(stack, swap_uint32(res));
    programCounter += 1;
}

void executeDup()
{
    word_t res = getTop(stack);
    push(stack, res);
    programCounter += 1;
}



void executeOut(){
  word_t res =  tos();
  fprintf(output_file,"0x%hhx 0x%hhx 0x%hhx 0x%hhx", (res >> 24 ) & 0xff, (res >> 16 ) & 0xff, (res >> 8 ) & 0xff, res & 0xff);
}
void run()
{
    // Step while you can
    for (int i=0; i<blt->dataSize; i++){
        step();
    }


  while(step());
}

bool step(){
  if(programCounter == blt->dataSize){
    return 0;
  }
  else{
    instruction = blt->data[programCounter];
    switch (instruction){
        case OP_BIPUSH:
            executeBipush();
            break;
        case OP_DUP:
            executeDup();
            break;
        case OP_IADD:
            executeIadd();
            break;
        case OP_ISUB:
            executeIsub();
            break;
        case OP_OUT:
            executeOut();
            break;
        default:
            break;
    }

    return 1;
  }

}

void set_input(FILE *fp)
{
  input_file = fp;
}

void set_output(FILE *fp)
{
  output_file = fp;
}
