#include <ijvm.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "util.h"
#include "BinaryLoader.h"
#include "Stack.h"
#include "Frame.h"

BinaryLoader_t *buffer;
Stack_t stack;
byte_t instruction;
Frame_t *frame;
int wideBoolean = 0;

int programCounter = 0;

FILE *output_file;
FILE *input_file;

int init_ijvm(char *binary_file)
{
    output_file = stdout;
    input_file = stdin;
    
    // Initialise the Stack, Frame
    createStack(&stack, 10000);
    frame = (Frame_t*)malloc(sizeof(Frame_t));
    // Implement loading of binary here
    buffer = (BinaryLoader_t *)malloc(sizeof(BinaryLoader_t));
    BinaryLoader(buffer, binary_file);

    if (checkMagicBytes(buffer) != 0)
    {
        return -1;
    }
    getConstants(buffer);
    getData(buffer);

    return 0;
}

int text_size()
{
    return buffer->dataSize;
}

byte_t *get_text()
{

    return buffer->data;
}

void destroy_ijvm()
{
    programCounter = 0;
    stackDestroy(&stack);
    // Reset IJVM state
    free(buffer->buffer);
    free(buffer->constants);
    free(buffer->data);
    free(buffer);
}

int get_program_counter() //how far in instructions
{
    return programCounter;
}

int tos() //Return top element of the stack, first swapping it and then returning it as an integer
{
    return (int)swap_uint32(getTop(&stack));
}

// TODO: add frame pointer
word_t *get_stack()
{
    return stack.array;
}

int shortToSignedInt(byte_t arg1, byte_t arg2)
{
    int result = (signed short int)((arg1 << 8) | (arg2 & 0xff));
    return result;
}

int shortToUnsignedInt(byte_t arg1, byte_t arg2)
{
    int result = (unsigned short int)(((arg1 & 0xFF) << 8)|(arg2 & 0xFF));
    return result;
}

void executeIload()
{
    word_t arg = buffer->data[programCounter + 1];
    if (wideBoolean == 1)
    {
        arg = (arg << 8 ) |buffer->data[programCounter + 2];
    }
    push (&stack, frame->localVariables[arg]);
    programCounter += 2;
    wideBoolean = 0;
   
}

void executeIstore()
{
    word_t arg1 = buffer->data[programCounter + 1];
    if (wideBoolean == 1)
    {
        arg1 = (arg1 << 8 ) | buffer->data[programCounter + 2];
    }
    byte_t arg2 = pop(&stack);
    frame->localVariables[arg1] = arg2;
    programCounter += 2;
    wideBoolean = 0;
}

void executeIinc()
{
    word_t arg1 = buffer->data[programCounter + 1];
    if (wideBoolean == 1)
    {
        arg1 = (arg1 << 8 ) | buffer->data[programCounter + 2];
    }
    byte_t arg2 = (int)buffer->data[programCounter + 3 - wideBoolean];
    frame->localVariables[arg1] += arg2;
    programCounter +=3;
    wideBoolean = 0;
}

void executeWide()
{
    int wideBoolean = 1;
}

void executeLdc_w()
{
    byte_t byte1 = buffer->data[programCounter + 1];
    byte_t byte2 = buffer->data[programCounter + 2];
    int res = shortToUnsignedInt(byte1, byte2);
    
    push(&stack, buffer->constants[res]);
    
    programCounter += 3;
}

void executeHalt()
{
    programCounter = buffer->dataSize;
}


void executeIfeq()
{
    int firstOperand = (int)swap_uint32(pop(&stack));
    
    if (firstOperand == 0)
    {
        byte_t byte1 = buffer->data[programCounter + 1];
        byte_t byte2 = buffer->data[programCounter + 2];
        int res = shortToSignedInt(byte1, byte2);
        programCounter += res;
    }
    else
    {
        programCounter += 3;
    }
}

void executeIflt()
{
    int firstOperand = (int)swap_uint32(pop(&stack));
    
    if (firstOperand < 0)
    {
        byte_t byte1 = buffer->data[programCounter + 1];
        byte_t byte2 = buffer->data[programCounter + 2];
        int res = shortToSignedInt(byte1, byte2);
        programCounter += res;
    }
    else
    {
        programCounter += 3;
    }
}

void executeIf_icmpeq()
{
    int firstOperand = (int)swap_uint32(pop(&stack));
    int secondOperand = (int)swap_uint32(pop(&stack));
    if (firstOperand == secondOperand)
    {
        byte_t byte1 = buffer->data[programCounter + 1];
        byte_t byte2 = buffer->data[programCounter + 2];
        int res = shortToSignedInt(byte1, byte2);
        programCounter += res;
    }
    else
    {
        programCounter += 3;
    }
}

word_t get_local_variable(int i)
{
    frame->localVariables[i];
}



word_t get_constant(int i)
{
    return  buffer->constants[i];
}

void executeGoto()
{
    byte_t byte1 = buffer->data[programCounter + 1];
    byte_t byte2 = buffer->data[programCounter + 2];
    int res = shortToSignedInt(byte1, byte2);
    programCounter += res;
}

word_t executePop()
{
    programCounter++;
    return pop(&stack);
}

void executeSwap()
{
    word_t temp = stack.array[stack.top - 1];
    stack.array[stack.top - 1] = stack.array[stack.top];
    stack.array[stack.top] = temp;
    programCounter += 1;
}

void executeBipush()
{
    int arg = (int8_t)(buffer->data[programCounter + 1]);
    push(&stack, swap_uint32(arg));
    programCounter += 2;
}

void executeIadd() //pop as big endian execute the add as little endian and push back in big endian
{
    word_t arg1 = pop(&stack);
    word_t arg2 = pop(&stack);
    word_t res = swap_uint32(arg1) + swap_uint32(arg2);

    push(&stack, swap_uint32(res));
    programCounter += 1;
}

void executeIsub()
{
    word_t arg1 = pop(&stack);
    word_t arg2 = pop(&stack);
    word_t res = swap_uint32(arg2) - swap_uint32(arg1);

    push(&stack, swap_uint32(res));
    programCounter += 1;
}
void executeIand()
{
    word_t arg1 = swap_uint32(pop(&stack)); //swap to little endian in order to and both args and then swap to push back in big endian
    word_t arg2 = swap_uint32(pop(&stack));
    word_t res = arg1 & arg2;

    push(&stack, swap_uint32(res));
    programCounter += 1;
}

void executeIor()
{
    word_t arg1 = swap_uint32(pop(&stack));
    word_t arg2 = swap_uint32(pop(&stack));
    word_t res = arg1 | arg2;

    push(&stack, swap_uint32(res));
    programCounter += 1;
}

void executeDup()
{
    word_t res = getTop(&stack);
    push(&stack, res);
    programCounter += 1;
}

void executeOut()
{
    word_t res = pop(&stack);
    fprintf(output_file, "0x%hhx 0x%hhx 0x%hhx 0x%hhx", (res >> 24) & 0xff, (res >> 16) & 0xff, (res >> 8) & 0xff, res & 0xff);
    programCounter += 1;
}
void run()
{
    printf("Datasize: %d\n",buffer->constantSize);
    while (step());
}

void printStack()//?
{
    printf("; stack ");
    for(int i = 0 ; i <= stack.top ;++i )
    {
        printf(" %x ",*(stack.array + i ));
     }
}
bool step()
{
    if (programCounter == buffer->dataSize)
    {
        printf("%d\n",programCounter);
        return 0;
    }
    else
    {
        
        instruction = buffer->data[programCounter];
        switch (instruction)
        {
        case OP_BIPUSH:
            executeBipush();
            break;
        case OP_POP:
            executePop();
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
        case OP_IAND:
            executeIand();
            break;
        case OP_IOR:
            executeIor();
            break;
        case OP_SWAP:
            executeSwap();
            break;
        case OP_OUT:
            executeOut();
            break;
        case OP_GOTO:
            executeGoto();
            break;
        case OP_IFEQ:
            executeIfeq();
            break;
        case OP_IFLT:
            executeIflt();
            break;
        case OP_ICMPEQ:
            executeIf_icmpeq();
            break;
        case OP_LDC_W:
            executeLdc_w();
            break;
        case OP_HALT:
            executeHalt();
            break;
        case OP_ILOAD:
            executeIload();
            break;
        case OP_ISTORE:
            executeIstore();
            break;
        case OP_IINC:
            executeIinc();
            break;
        case OP_WIDE:
            executeWide();
            break;
        default:
            break;
        
        }
        printStack();
        
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
