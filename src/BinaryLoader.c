//
//  BinaryLoader.c
//  Created by JonathanOsterman on 07/06/2018.
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define STEP 4
//Build a struct that contains all the bytes of the binary file as well as all the structure variables
typedef struct BinaryLoader{
    
    unsigned char *buffer;
    unsigned char *constants;
    unsigned char *data;
    
    int constantSize;
    int dataSize;
}BinaryLoader_t;

//Function to open the file and load the bytes into the buffer  of the buff argument
int BinaryLoader (BinaryLoader_t *buff, char *fileName){
    
    FILE *file;
    unsigned long fileLen;
    //Open file
    file = fopen(fileName, "rb");
    if (!file){
        fprintf(stderr, "Unable to open file %s", fileName);
    }
    //Get file length
    fseek(file, 0, SEEK_END);
    fileLen=ftell(file);
    
    if  (fileLen < 4){
        return -1;
    }
    
    fseek(file, 0, SEEK_SET);
    //Allocate memory
    buff->buffer = (unsigned char *)malloc(fileLen + 1);
    if (!buff->buffer){
        fprintf(stderr, "Memory error!");
        fclose(file);
    }
    //Read file contents into buffer
    fread(buff->buffer, fileLen, 1, file);
    fclose(file);
    return 0; 
}
    
//Function to check if the binary file is a correct IJVM file
int checkMagicBytes(BinaryLoader_t *buff){
    
        unsigned char magicBytes[4] = {0x1d, 0xea, 0xdf, 0xad};
        
        for (int i=0; i<4; i++) {
            if(buff->buffer[i] != magicBytes[i])
            {
                return -1;            
            }   
        }
        return 0;
    }

//Function to transform Hex to Integer
int changeToDecimal(BinaryLoader_t *buff, int i){
    
    int result = (unsigned char)buff->buffer[i] * 65536 + (unsigned char)buff->buffer[i + 1] * 4096 + (unsigned char)buff->buffer[i + 2] * 256 + (unsigned char)buff->buffer[i + 3];
    
            return result;
    }

//Function to retrieve the constants
void getConstants (BinaryLoader_t *buff) {
    int constantSizeStart = 2 * STEP;
    int constantDataStart = constantSizeStart + STEP;
    
    buff->constantSize = changeToDecimal(buff, constantSizeStart);
    buff->constants = (unsigned char *)malloc(buff->constantSize + 1);
    
    for (int i=0; i<buff->constantSize; i++){
            buff->constants[i] = buff->buffer[i + constantDataStart];
        }
}

//Function to retrieve the data
void getData (BinaryLoader_t *buff){
    int dataSizeStart = 12 + buff->constantSize + STEP;
    int dataStart = dataSizeStart + STEP;
    
    buff->dataSize = changeToDecimal(buff, dataSizeStart);
    buff->data = (unsigned char *)malloc(buff->dataSize + 1);
    
    for (int i=0; i<buff->dataSize; i++) {
            buff->data[i] = buff->buffer[i+dataStart];
        }
}

