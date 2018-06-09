#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/resource.h>
#include <time.h>

#define NUMBER_OF_THREADS 20


int array[NUMBER_OF_THREADS][70];
pthread_t threads[NUMBER_OF_THREADS];

void *routine(void *args) {
	int i;
    int thread_number = *(int *) args;
	sleep(2);
	for (i = 0; i < 70; i++){
		array[thread_number][i] = i;
	}
    return NULL;
}


int main() {
	int i, j;

	// uruchamianie watkow
	for (i = 0; i < NUMBER_OF_THREADS; i++){
		int *arg = malloc(sizeof(int));
		*arg = i;
		pthread_create(&threads[i], NULL, routine, arg);
	}

	// czekanie na watki
	for(i = 0; i < NUMBER_OF_THREADS; i++)
  		pthread_join(threads[i], NULL);

	// wyswietlanie tablicy
	for (i = 0 ; i < NUMBER_OF_THREADS; i++){
		for (j = 0 ; j < 70; j++){
			printf("%d ", array[i][j]);		
		}
		printf("\n");
	}
}
	
