#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "libzad1.h"

static int staticArray[1000][1000];

//FUNCTIONS
int abs(int l1){
    if (l1 < 0) return -l1;
    else return l1;
}
int difference(int l1, int l2){
    return abs(l1-l2);
}
int blockOfCharsToInt(char *block, int lengthOfBlock){
    int i, sum = 0;
    for (i=0; i<lengthOfBlock; i++){
        sum += block[i] - '0';
    }
    return sum;
}
char * searchNearestCharNumberInDynamicArray(struct dynamicArray * array, int number){
    int diff, i, j, tmpDiff;
    if (array -> occupied == 0) return NULL;
    if (array -> occupied == 1) return array -> chars[0];

    diff = difference(blockOfCharsToInt(array->chars[0], array->lengthsOfBlocks[0]), number);
    j = 0;
    for (int i = 1; i < array -> size; i++){
        tmpDiff = difference(blockOfCharsToInt(array->chars[i], array->lengthsOfBlocks[i]), number);
        if (tmpDiff < diff)
            j = i;
    }
    return array -> chars[j];
}
char randomCharGenerator(){
    char * string = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";
    return string[rand()%69];
}
void randomDynamicArrayOfCharsGenerator(struct dynamicArray *array){
    int i, j;
    for (i = 0; i < array -> size; i++)
        for (int j = 0; j < array -> lengthsOfBlocks[i]; j++)
            array->chars[i][j] = randomCharGenerator();
}
void printDynamicArray(struct dynamicArray *array){
    int i, j;
    for (i = 0; i < array -> size; i++) {
        for (int j = 0; j < array->lengthsOfBlocks[i]; j++) {
            printf("%c ", array->chars[i][j]);
        }
        printf("\n");
    }
}
int stringLenght(char * string){
    int lenght = 0;
    while (string[lenght] != 0)
        lenght++;
    return lenght;
}

//STATIC
void static2DArrayInitialization(int length, int height){
    int i, j;
    for (i = 0; i < length; i++)
        for (j = 0; j < height; j++)
            staticArray[i][j] = randomCharGenerator();
}
void static2DArrayInitializationAndSearch(int length, int height, int number){
    int i, j, sum, diff, diffIndex, tmpDiff;
    int staticArray[length][height];
    for (i = 0; i < length; i++) {
        for (j = 0; j < height; j++) {
            staticArray[i][j] = randomCharGenerator();
        }
    }
    sum = 0;
    i = 0;
    for (j = 0; j < height; j++) {
        sum = sum + staticArray[i][j];
    }
    diff = difference(sum, number);

    for (i = 1; i < length; i++) {
        sum = 0;
        for (j = 0; j < height; j++) {
            sum = sum + staticArray[i][j];
        }
        tmpDiff = difference(number, sum);
        if (tmpDiff < diff){
            diff = tmpDiff;
            diffIndex = i;
        }
    }
}

//DYNAMIC
void initDynamic2DArray(struct dynamicArray * array, int lengthOfArray){
    array->chars = (char**) calloc(lengthOfArray, sizeof(char*));
    array->lengthsOfBlocks = (int*) calloc(lengthOfArray, sizeof(int));
    array->size = lengthOfArray;
    array->occupied = 0;
}
void addNewBlock(struct dynamicArray * array, int blockLength){
    if (array->size > array->occupied) {
        array->chars[array->occupied] = calloc(blockLength, sizeof(char));
        array->lengthsOfBlocks[array->occupied] = blockLength;
        array->occupied++;
    }
}
void freeArray(struct dynamicArray * array){
    free(array->chars);
    array->occupied = 0;
}
void freeBlock(struct dynamicArray * array){
    array->occupied--;
    free(array->chars[array->occupied]);
}
