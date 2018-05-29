#include "common.h"

int ANTEROOM_SIZE;
struct antreroom *hairdressers_antreroom;

void get_semaphore(int semaphore_id) {
    struct sembuf semaphore_request;
    semaphore_request.sem_num = 0;
    semaphore_request.sem_op = -1;
    semaphore_request.sem_flg = 0;

    if (semop(semaphore_id, &semaphore_request, 1)){
        printf("Client: Could not update semaphore\n");
        return;
    }
}

void release_semaphore(int semaphore_id) {
    struct sembuf semaphore_request;
    semaphore_request.sem_num = 0;
    semaphore_request.sem_op = 1;
    semaphore_request.sem_flg = 0;

    if (semop(semaphore_id, &semaphore_request, 1)){
        printf("Client: Could not update semaphore\n");
        return;
    }
}

bool find_free_chair(pid_t pid) {
    printf("Client %i: Lets look for free chair in atreroom.\n", (int) pid);
    if (hairdressers_antreroom->occupied == ANTEROOM_SIZE) {
        printf("Client %i: No free chairs, I will come tomorrow!\n", pid);
        return false;
    }
    return true;
}

void occupie_chair(pid_t pid) {
    printf("Client %i: Let's take a sit %d and wait for barber. \n", (int) pid,
           hairdressers_antreroom->occupied + 1);
    hairdressers_antreroom->chairs[hairdressers_antreroom->occupied] = pid;
    hairdressers_antreroom->occupied++;
}

bool check_if_sleeping(pid_t pid) {
    if (hairdressers_antreroom->chairs[0] == pid)
        printf("Client %i: I'm waiting here so long... Let's check what is going on with barber.\n", (int) pid);
    else printf("Client %i: I have to wait for my turn.\n", (int) pid);
    if (hairdressers_antreroom->sleeping == 1)
        return true;
    return false;
}

void wake_up(pid_t pid) {
    printf("Client %i: Ohh! Wake up man. People are waiting!\n", pid);
    hairdressers_antreroom->sleeping = 0;
}

void prepare_shared_memory() {
    key_t project_key = ftok(shmPath, project_id);
    shared_memory_id = shmget(project_key, sizeof(struct antreroom), 0);
    if (shared_memory_id == -1) {
        printf("Client: Opening shared memory failed!");
        return;
    }

    void *tmp = shmat(shared_memory_id, 0, 0);
    hairdressers_antreroom = (struct antreroom *) tmp;
}

void prepare_semaphores() {
    key_t project_key = ftok(shmPath, project_id);
    semaphore_id = semget(project_key, 0, 0);
    if (semaphore_id == -1) {
        printf("Client: Couldn't create semaphore\n");
        return;
    }
}

int main(int argc, char *argv[]) {
    ANTEROOM_SIZE = atoi(argv[1]);
    prepare_semaphores();
    prepare_shared_memory();

    pid_t pid = getpid();
    printf("Client %d: I'm waiting for semaphore.\n", pid);
    get_semaphore(semaphore_id);
    printf("Client %d: I got semaphore!\n", getpid());
    if (find_free_chair(pid)) {
        occupie_chair(pid);
        if (check_if_sleeping(pid)) {
            wake_up(pid);
        }
    }
    printf("Client %d: I released semaphore.\n", getpid());
    release_semaphore(semaphore_id);
}

