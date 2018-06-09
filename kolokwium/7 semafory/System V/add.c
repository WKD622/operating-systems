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
        printf("add: Could not update semaphore\n");
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
        printf("add: Could not update semaphore\n");
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


	// tworzenie semafora
	project_key = ftok(sem_path, PROJECT_ID);
	if (project_key == -1) {
		perror("ftok sem");
		return -1;
	}
   	smaphore_id = semget(project_key, 1, IPC_CREAT | S_IRWXU);
    	if (semaphore_id == -1) {
        	printf("add: Couldn't create semaphore\n");
        	return -1;
    	}
    	semctl(semaphore_id, 0, SETVAL, 1);


	// tworzenie współdzielonej pamięci
	project_key = ftok(shm_path, PROJECT_ID);
    	if (project_key == -1) {
        	printf("shm add: Couldn't obtain a project key\n");
        	return -1;
    	}
    	shared_memory_id = shmget(project_key, sizeof(struct common_memory), S_IRWXU | IPC_CREAT);
    	if (shared_memory_id == -1) {
		printf("add: Couldn't create shared memory\n");
        	return -1;
    	}
    	void *tmp = shmat(shared_memory_id, 0, 0);
    	common_memory1 = (struct common_memory *) tmp;

	
	// ustawienie pamieci
	common_memory1->value = 0;


	// wywolanie
	pid_t pid = fork();
	if (pid == 0)
		execl("./remove", "./remove", NULL);
	else{
		while(1) {
			get_semaphore(semaphore_id);
			printf("add: I took sempahore\n");
			if (common_memory1->value == 0){
				common_memory1->value = 1;
				printf("add: I add value %d to shared memory\n", common_memory1->value);
			}
			printf("add: I relese sempahore\n");
			release_semaphore(semaphore_id);
		}
	}

	return 1;
}
