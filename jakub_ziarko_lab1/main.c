#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <dlfcn.h>
#include "libzad1.h"

/*
 *a: stworzenie tablicy, wyszukanie elementu oraz usunięcie i dodanie zadanej liczby bloków
 *b: albo stworzenie tablicy, usunięcie i dodanie zadanej liczby bloków i naprzemienne usunięcie i dodanie zadanej liczby bloków).
 */

/*
   DynamicOrStatic == 1 - dynamic allocation 
   else static allocation
*/

//STRUCTURES
struct timeMeasurment{
    float kernelTime;
    float userTime;
};

//FUNCTIONS
float sumMicroSecAndSec(int sec, int microSec){
    return (float) microSec / 1000000 + (float) sec;
}
void measureTime(struct timeMeasurment * mes, char * message, FILE *raport){
    struct timeval user;
    struct timeval kernel;
    struct rusage rusage1;
    float secKernel, secUser;
    
    getrusage(RUSAGE_SELF, &rusage1);
    user = rusage1.ru_utime;
    kernel = rusage1.ru_stime;
    secKernel = sumMicroSecAndSec(kernel.tv_sec, kernel.tv_usec);
    secUser = sumMicroSecAndSec(user.tv_sec, user.tv_usec);
    printf("%s kernel: %fs   |   user: %fs\n\n", message, secKernel - mes->kernelTime, secUser - mes->userTime);
    fprintf(raport, "%s kernel: %fs   |   user: %fs\n\n", message, secKernel - mes->kernelTime, secUser - mes->userTime);
    mes->kernelTime = secKernel;
    mes->userTime = secUser;
}
void printfTextOnConsoleAndFile(char * text, FILE * raport){
    fprintf(raport, "%s", text);
    printf("%s",text);
}
void macros(FILE * raport){
    #ifdef STATIC_LIB
	    printf("\nStatic library\n\n");
        fprintf(raport, "Static library\n\n");
    #endif

    #ifdef SHARED_LIB
	    printf("\nShared library\n\n");
        fprintf(raport, "Shared library\n\n");
    #endif

    #ifdef DYNAMIC_LIB
	    printf("\nDynamic library\n\n");
        fprintf(raport, "Dynamic library\n\n");
    #endif

}
void exercise2(int dynamicAlloc, int staticAlloc, int arrayLength, int blockSize, char * instructions, int repeats, int valueForSearch){

     #ifdef DYNAMIC_LIB
        char *dlerror(void);
        void *handle = dlopen("libzad1.so", RTLD_LAZY);
        if(!handle){/*error*/}
        
        int (*abs)(int l1);
        abs = (int (*)())dlsym(handle,"abs");
        
        int (*difference)(int l1, int l2);
        difference = (int (*)())dlsym(handle,"difference");
        
        int (*blockOfCharsToInt)(char *block, int lengthOfBlock);
        blockOfCharsToInt = (int (*)())dlsym(handle,"blockOfCharsToInt");
        
        char * (*searchNearestCharNumberInDynamicArray)(struct dynamicArray * array, int number);
        searchNearestCharNumberInDynamicArray = (char * (*)())dlsym(handle,"searchNearestCharNumberInDynamicArray");
        
        char (*randomCharGenerator)();
        randomCharGenerator = (char (*)())dlsym(handle,"randomCharGenerator");
        
        void (*randomDynamicArrayOfCharsGenerator)(struct dynamicArray *array);
        randomDynamicArrayOfCharsGenerator = (void (*)())dlsym(handle,"randomDynamicArrayOfCharsGenerator");
        
        void (*printDynamicArray)(struct dynamicArray *array);
        printDynamicArray = (void (*)())dlsym(handle,"printDynamicArray");
        
        int (*stringLenght)(char * string);
        stringLenght = dlsym(handle,"stringLenght");
        
        void (*static2DArrayInitialization)(int length, int heigth);
        static2DArrayInitialization = (void (*)())dlsym(handle,"static2DArrayInitialization");
        
        void (*static2DArrayInitializationAndSearch)(int length, int heigth, int number);
        static2DArrayInitializationAndSearch = (void (*)())dlsym(handle,"static2DArrayInitializationAndSearch");
        
        void (*initDynamic2DArray)(struct dynamicArray * array, int lengthOfArray);
        initDynamic2DArray = (void (*)())dlsym(handle,"initDynamic2DArray");
        
        void (*addNewBlock)(struct dynamicArray * array, int blockLength);
        addNewBlock = (void (*)())dlsym(handle,"addNewBlock");
        
        void (*freeArray)(struct dynamicArray * array);
        freeArray = (void (*)())dlsym(handle,"freeArray");
        
        void (*freeBlock)(struct dynamicArray * array);
        freeBlock = (void (*)())dlsym(handle,"freeBlock");
               
    #endif

    int countInstructions = stringLenght(instructions);
    FILE *raport = fopen("raport.txt", "w");
    struct timeMeasurment mes;
    int i, j, k;
    struct dynamicArray array;
    struct timeval user;
    struct timeval kernel;
    struct rusage rusage1;
    float startUserTime, startKernelTime, midUserTime, midKernelTime;
    clock_t t;

    macros(raport);
    t = clock();
    getrusage(RUSAGE_SELF, &rusage1);
    user = rusage1.ru_utime;
    kernel = rusage1.ru_stime;
    mes.kernelTime = sumMicroSecAndSec(kernel.tv_sec, kernel.tv_usec);
    mes.userTime = sumMicroSecAndSec(user.tv_sec, user.tv_usec);
    startKernelTime = mes.kernelTime;
    startUserTime = mes.userTime;

    if (dynamicAlloc == 1){
        for (k = 0; k < repeats; k++) {
            for (i = 0; i < countInstructions; i++){
                switch (instructions[i]){
                    case 'a':
                        initDynamic2DArray(&array, arrayLength);
                        for (j = 0; j < arrayLength; j++)
                            addNewBlock(&array, blockSize);
                        randomDynamicArrayOfCharsGenerator(&array);
                        searchNearestCharNumberInDynamicArray(&array, valueForSearch);
                        freeArray(&array);
                        break;
                    case 'b':
                        initDynamic2DArray(&array, arrayLength);
                        for (j = 0; j < arrayLength; j++)
                            addNewBlock(&array, blockSize);
                        break;
                }
            }
        }
    }
    measureTime(&mes, "AFTER ALL OPERATIONS ON DYNAMIC ALLOCATED ARRAY\n", raport);

    if(staticAlloc == 1){ 
        for (k = 0; k < repeats; k++) {    
            for (i = 0; i < countInstructions; i++){
                switch (instructions[i]){
                    case 'a':
                        static2DArrayInitializationAndSearch(arrayLength, blockSize, valueForSearch); 
                        break;
                    case 'b':
                        static2DArrayInitialization(arrayLength, blockSize);
                        break;
                }
            }
        }
    }
    
    measureTime(&mes, "AFTER ALL OPERATIONS ON STATIC ALLOCATED ARRAY\n", raport);
    t = clock() - t;
    mes.kernelTime = startKernelTime;
    mes.userTime = startUserTime;
    printfTextOnConsoleAndFile("STATS FOR WHOLE PROGRAM:\n",raport);
    printf(" real: %fs   |   ", ((double)t)/CLOCKS_PER_SEC);
    fprintf(raport, " real: %fs   |   ", ((double)t)/CLOCKS_PER_SEC);
    measureTime(&mes, "", raport);
}

//MAIN
int main(){
    srand(time(0));
    exercise2(1, 1, 1000, 1000, "abbabbaababaa", 100, 151);
}
