#ifndef ZADANIE1_LIBRARY_H
#define ZADANIE1_LIBRARY_H

//STRUCTURES
struct dynamicArray{
    int size;
    int occupied;
    char ** chars;
    int * lengthsOfBlocks;
};

//FUNCTIONS
int abs(int l1);
int difference(int l1, int l2);
int blockOfCharsToInt(char *block, int lengthOfBlock);
char * searchNearestCharNumberInDynamicArray(struct dynamicArray * array, int number);
char randomCharGenerator();
void randomDynamicArrayOfCharsGenerator(struct dynamicArray *array);
void printDynamicArray(struct dynamicArray *array);
int stringLenght(char * string);

//STATIC
void static2DArrayInitialization(int length, int heigth);
void static2DArrayInitializationAndSearch(int length, int heigth, int number);

//DYNAMIC
void initDynamic2DArray(struct dynamicArray * array, int lengthOfArray);
void addNewBlock(struct dynamicArray * array, int blockLength);
void freeArray(struct dynamicArray * array);
void freeBlock(struct dynamicArray * array);

#endif