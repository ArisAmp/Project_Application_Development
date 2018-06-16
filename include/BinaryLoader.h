#ifndef HEADER_BINARYLOADER
#define HEADER_BINARYLOADER

typedef struct BinaryLoader{
    
    unsigned char *buffer;
    unsigned char *constants;
    unsigned char *data;
    
    int constantSize;
    int dataSize;
}BinaryLoader_t;

int BinaryLoader (BinaryLoader_t *buff, char *fileName);
int checkMagicBytes(BinaryLoader_t *buff);
int changeToDecimal(BinaryLoader_t *buff, int i);
void getConstants (BinaryLoader_t *buff);
void getData (BinaryLoader_t *buff);

#endif
