#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdbool.h>
	
struct common_memory{
	int value;
};

int main(){
	struct common_memory *common_memory1;

	// tworzenie semafora
	sem_t *SEMAPHORE = sem_open("/semaphore", O_RDWR | O_CREAT, 0777, 0);
	if (SEMAPHORE == SEM_FAILED){
        printf("add: Creating semaphore failed!\n");
        return -1;
    }
	sem_post(SEMAPHORE);


	// tworzenie współdzielonej pamięci
	int shmID = shm_open("/shm", O_RDWR | O_CREAT, 0666);
    if (shmID == -1) printf("add: Creating shared memory failed!\n");
    ftruncate(shmID, sizeof(struct common_memory));
	void *tmp = mmap(NULL, sizeof(struct common_memory), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
	common_memory1 = (struct common_memory *) tmp;

	
	// wywołanie
	pid_t pid = fork();
	if (pid == 0)
		execl("./remove", "./remove", NULL);
	else{
		while(1) {
			sem_wait(SEMAPHORE);
			printf("add: I took sempahore\n");
			if (common_memory1->value == 0){
				common_memory1->value = 1;
				printf("add: I add value %d to shared memory\n", common_memory1->value);
			}
			printf("add: I relese sempahore\n");
			sem_post(SEMAPHORE);
		}
	}

	return 1;
}
