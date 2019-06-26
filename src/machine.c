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
    //Set files to STDOUT, STDIN
    output_file = stdout;
    input_file = stdin;
    
    // Initialise the Stack, allocate memory for Frame
    createStack(&stack, 100000);
    frame = (Frame_t *)malloc(sizeof(Frame_t));
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
//Return the byte of the instruction
byte_t get_instruction()
{
    return buffer->data[programCounter];
}

int stack_size()
{
    return stack.size;
}

//Return the size of the text(data)
int text_size()
{
    return buffer->dataSize;
}

//Return actual data
byte_t *get_text()
{
    return buffer->data;
}

// Reset IJVM state
void destroy_ijvm()
{
    programCounter = 0;
    stackDestroy(&stack);
    free(buffer->constants);
    free(buffer->data);
    free(buffer);
    if(frame != NULL){
       free(frame);
    }
}

//How far in instructions
int get_program_counter()
{
    return programCounter;
}

//Return top element of the stack, first swapping it and then returning it as an integer
word_t tos()
{
    return (word_t)getTop(&stack);
}

//Return the stack
word_t *get_stack()
{
    return stack.array;
}

//Return the local variable at index i
word_t get_local_variable(int i)
{
    
    return frame->localVariables[i];
}

//Return the constant at index i
word_t get_constant(int i)
{
    return  buffer->constants[i];
}


//ILOAD: Fetch first arguement as the index and push the local variable at that index to the stack.
void executeIload()
{
    word_t index = buffer->data[programCounter + 1];
    push(&stack,  frame->localVariables[index]);
    programCounter += 2;
}

//ILOAD WIDE: The fetched arguements are transform to an int since they are 16bits and afterwards it pushes the variable at the tranformed index in local variables to the stack.
void executeIloadWide()
{
    byte_t arg = buffer->data[programCounter + 1];
    byte_t arg1 = buffer->data[programCounter + 2];
    int index = shortToUnsignedInt(arg,arg1);
    push(&stack, frame->localVariables[index]);
    programCounter += 3;
}


//ISTORE: Fetch first arguement,which is the index and if wide is enabled, bit shifts 8 bits to the left. finally it pops from the stack and places that value to local variables at the fetched index.
void executeIstore()
{
    byte_t index = buffer->data[programCounter + 1];
    
    word_t arg = pop(&stack);
    frame->localVariables[index] = arg;
    programCounter += 2;
}



//ISTORE WIDE: The fetched arguements are transformed to an int since they are 16bits and afterwards it pops from the stack and places value to local variables at the fetched transformed index.
void executeIstoreWide()
{
    byte_t arg = buffer->data[programCounter+1];
    byte_t arg1 = buffer->data[programCounter+2];
    printf("ARGS %d %d:", arg, arg1);

    unsigned int index = shortToUnsignedInt(arg,arg1);
    word_t arg2 = tos();
    pop(&stack);
    frame->localVariables[index] = arg2;
    printf("INDEX %d\n:", index );
    programCounter += 3;
}

//IINC: Fetch first arguement,which is the index, and if wide is enabled, bit shifts 8 bits to the left. finally it casts the second arguement as a signed char and adds that value to the index in local variables.
void executeIinc()
{
    word_t index = buffer->data[programCounter + 1];
    char arg = (char)buffer->data[programCounter + 2];
    frame->localVariables[index] += arg;
    programCounter +=3;
    wideBoolean = 0;
}

//IINC WIDE: The first two fetched arguements are transformed to an in which is the index to the local variables and the next two arguements are transformed to an int which is the index to the data. This word is casted as a signed char and added to the local variables
void executeIincWide()
{
    byte_t arg = buffer->data[programCounter+1];
    byte_t arg1 = buffer->data[programCounter+2];
    byte_t arg2 = buffer->data[programCounter+3];
    byte_t arg3 = buffer->data[programCounter+4];
    
    int index = shortToUnsignedInt(arg,arg1);
    int index1 = shortToUnsignedInt(arg2,arg3);
    char finalArg = (char)buffer->data[index1];
    
    frame->localVariables[index] += finalArg;
    programCounter += 5;
}

//INVOKEVIRTUAL: Linked list. Create a new "node"(tempFrame) and store the current frame in the "previous" frame struct that there is inside the new node. Then set the current frame to the temp one and store the program counter inside the struct for returning to the right instruction.
void executeInvokeVirtual()
{
    Frame_t *tempFrame;
    tempFrame = (Frame_t*)malloc(sizeof(Frame_t));
    tempFrame->previous = frame;
    frame = tempFrame;
    frame->framePC = programCounter + 3;//for return
    

    byte_t arg1 = buffer->data[programCounter + 1];//Fetch the short bytes and transform it to an integer
    byte_t arg2 = buffer->data[programCounter + 2];//which is the index to the constants.
    int index = shortToSignedInt(arg1,arg2);
    
    programCounter = (int)buffer->constants[index*4];//Set the programm counter to the fetched constant in                         //order to continue execution
    
    byte_t arg3 = buffer->data[programCounter]; //Next short bytes are combined and transform to an int
    byte_t arg4 = buffer->data[programCounter + 1];//which is the number of arguements
    int numberOfArgs = shortToUnsignedInt(arg3, arg4);
    for (int i = numberOfArgs - 1 ; i > 0; i--) { //Poping from the stack to the first local variables
                                                  //indicated by number of arguements
        frame->localVariables[i] = pop(&stack);
    }
    pop(&stack);
    
    int localVariableAreaSize = shortToUnsignedInt(buffer->data[programCounter + 2],buffer->data[programCounter + 3]); //not used yet
    programCounter += 4;
}

//IRETURN: Resets the program counter to the previous one stored in framePC in order to return correctly, pop the return value from the stack, return to the previous frame push the returned value to the stack and destroy the old frame.
void executeIreturn()
{
    programCounter = frame->framePC;
    word_t returnValue = pop(&stack);
    Frame_t * oldFrame = frame;
    frame = oldFrame->previous;
    push(&stack, returnValue);
    free(oldFrame);
}

//WIDE: Simply go to the next instruction and do the appropriate WIDE ILOAD, WIDE ISTORE, WIDE IINC.
void executeWide()
{
    programCounter += 1;
    instruction = buffer->data[programCounter];
    switch (instruction){
        case OP_ILOAD:
            executeIloadWide();
            break;
        case OP_ISTORE:
            executeIstoreWide();
            break;
        case OP_IINC:
            executeIincWide();
            break;
    }
}

//LDC_W: Transform the short byte to an int index, in order to push the constant at that index(multiplied by 4 because every constant is 4bytes)from the pool to the stack.
void executeLdc_w()
{
    byte_t byte1 = buffer->data[programCounter + 1];
    byte_t byte2 = buffer->data[programCounter + 2];
    
    int index = shortToUnsignedInt(byte1, byte2) * 4;
    word_t res = buffer->constants[index];
    push(&stack, res);
    programCounter += 3;
}

//HALT: Sets the program counter to the data size(end of text) in order to pause the execution.
void executeHalt()
{
    programCounter = buffer->dataSize;
}

//NOP: Go to next instruction
void executeNop()
{
    programCounter += 1;
}

//IFEQ: Pops word as an int from the stack. If it is zero transforms the short arguement to an int and adds it to the program counter in order to branch. Else it continues to the next instruction.
void executeIfeq()
{
    int firstOperand = (int)pop(&stack);
    
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

//IFLT: Pops word as an int from the stack. If it is lower than zero transforms the short arguement to an int and adds it to the program counter in order to branch. Else it continues to the next instruction.
void executeIflt()
{
    int firstOperand = (int)pop(&stack);
    
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

//IF_ICMPEQ: Pops two words from stack as ints. If they are equal transforms the short arguement to an int and adds it to the program counter in order to branch. Else it continues to the next instruction.
void executeIf_icmpeq()
{
    int firstOperand = (int)pop(&stack);
    int secondOperand = (int)pop(&stack);
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


//GOTO: Transforms the short arguement to an int and adds it to the program counter in order to branch.
void executeGoto()
{
    byte_t byte1 = buffer->data[programCounter + 1];
    byte_t byte2 = buffer->data[programCounter + 2];
    int res = shortToSignedInt(byte1, byte2);
    programCounter += res;
}

//POP: Pops from the stack.
word_t executePop()
{
    programCounter += 1;
    pop(&stack);
}
//SWAP: Swap the two top words in the stack.
void executeSwap()
{
    word_t temp = stack.array[stack.top - 1];
    stack.array[stack.top - 1] = stack.array[stack.top];
    stack.array[stack.top] = temp;
    programCounter += 1;
}

//BIPUSH: Pushes byte onto the stack.
void executeBipush()
{
    int8_t arg = (int8_t)(buffer->data[programCounter + 1]);
    push(&stack, arg);
    programCounter += 2;}

//IADD: Pops two words adds them and push the sum back to the stack.
void executeIadd()
{
    word_t arg1 = pop(&stack);
    word_t arg2 = pop(&stack);
    word_t res = arg1 + arg2;

    push(&stack, res);
    programCounter += 1;
}

//ISUB: Pops two words subtracts them and push the difference back to the stack.
void executeIsub()
{
    word_t arg1 = pop(&stack);
    word_t arg2 = pop(&stack);
    word_t res = arg2 - arg1;

    push(&stack, res);
    programCounter += 1;
}

//IAND: Pops two words ANDS them and push the answer back to the stack.
void executeIand()
{
    word_t arg1 = pop(&stack);
    word_t arg2 = pop(&stack);
    word_t res = arg1 & arg2;

    push(&stack, res);
    programCounter += 1;
}
//IOR: Pops two words ANDS them and push the answer back to the stack.
void executeIor()
{
    word_t arg1 = pop(&stack);
    word_t arg2 = pop(&stack);
    word_t res = arg1 | arg2;

    push(&stack, res);
    programCounter += 1;
}

//DUP: Gets the top of the stuck and pushes it again to the stack.
void executeDup()
{
    word_t res = getTop(&stack);
    push(&stack, res);
    programCounter += 1;
}

//OUT: Pops word from the stack and write it on the output file as a character.
void executeOut()
{
    word_t res = pop(&stack);
    
    fprintf(output_file, "%c", res);
    programCounter += 1;
}

//IN: Gets character from the file and pushes it to the stack.
void executeIn()
{
    int input = fgetc(input_file);
    
    if (input == EOF)
    {
        push(&stack, 0);
    }
    else
    {
        push(&stack, input);
    }
    programCounter += 1;
}

//RUN: Runs the program until halt
void run()
{
    while (step());
}

void printStack()//check
{
    printf("; stack ");
    for(int i = 0 ; i <= stack.top ;++i )
    {
        printf(" %x \n",*(stack.array + i ));
     }
}


//Switches the current instruction and executes the appropriate case. When it reaches the data size it ends.
bool step()
{
    if (programCounter == buffer->dataSize)
    {
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
        case OP_INVOKEVIRTUAL:
            executeInvokeVirtual();
            break;
        case OP_IRETURN:
            executeIreturn();
            break;
        case OP_NOP:
            executeNop();
            break;
        case OP_IN:
            executeIn();
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
