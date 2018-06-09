#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>
	
#define PROJECT_ID 123456789	

struct common_memory{
	int value;
};


// pobieranie semafora
void get_semaphore(int semaphore_id) {
    struct sembuf semaphore_request;
    semaphore_request.sem_num = 0;
    semaphore_request.sem_op = -1;
    semaphore_request.sem_flg = 0;

    if (semop(semaphore_id, &semaphore_request, 1)){
        printf("remove: Could not update semaphore\n");
        return;
    }
}


// oddawanie semafora
void release_semaphore(int semaphore_id) {
    struct sembuf semaphore_request;
    semaphore_request.sem_num = 0;
    semaphore_request.sem_op = 1;
    semaphore_request.sem_flg = 0;

    if (semop(semaphore_id, &semaphore_request, 1)){
        printf("remove: Could not update semaphore\n");
        return;
    }
}

int main(){
	const char *sem_path = "add.c";
	const char *shm_path = "remove.c";
	int shared_memory_id;
	int semaphore_id;
	key_t project_key;
	struct common_memory *common_memory1;


	// otwieranie semafora
	project_key = ftok(sem_path, PROJECT_ID);
    semaphore_id = semget(project_key, 0, 0);
    if (semaphore_id == -1) {
        printf("remove: Couldn't open semaphore\n");
        return -1;
    }


	// otwieranie współdzielonej pamięci
	project_key = ftok(shm_path, PROJECT_ID);
    shared_memory_id = shmget(project_key, sizeof(struct common_memory), 0);
    if (shared_memory_id == -1) {
        printf("Barber: Opening shared memory failed!");
        return -1;
    }
    void *tmp = shmat(shared_memory_id, 0, 0);
    common_memory1 = (struct common_memory *) tmp;


	// wywolanie
	while(1) {
		get_semaphore(semaphore_id);
		printf("remove: I took sempahore\n");
		if (common_memory1->value != 0){
			common_memory1->value = 0;
			printf("remove: I add value %d to shared memory\n", common_memory1->value);
		}
		printf("remove: I release sempahore\n");
		release_semaphore(semaphore_id);
	}

	return 1;
}
