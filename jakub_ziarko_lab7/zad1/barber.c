#include "common.h"

int ANTEROOM_SIZE;
struct antreroom *hairdressers_antreroom;

void get_semaphore(int semaphore_id) {
    struct sembuf semaphore_request;
    semaphore_request.sem_num = 0;
    semaphore_request.sem_op = -1;
    semaphore_request.sem_flg = 0;

    if (semop(semaphore_id, &semaphore_request, 1)){
        printf("Barber: Could not update semaphore\n");
        return;
    }
}

void release_semaphore(int semaphore_id) {
    struct sembuf semaphore_request;
    semaphore_request.sem_num = 0;
    semaphore_request.sem_op = 1;
    semaphore_request.sem_flg = 0;

    if (semop(semaphore_id, &semaphore_request, 1)){
        printf("Barber: Could not update semaphore\n");
        return;
    }
}

bool check_antreroom_empty() {
    printf("Barber: Let's check if someone waits in antreroom.\n");
    if (hairdressers_antreroom->occupied == 0)
        return true;
    return false;
}

void go_to_sleep() {
    printf("Barber: Yes! Empty antreroom, I'm going to sleep.\n");
    hairdressers_antreroom->sleeping = 1;
}

void take_for_cutting() {
    int cut_pid = (int) hairdressers_antreroom->chairs[0];
    printf("Barber: Welcome to the hairdresser %i. Please take your sit.\n", cut_pid);
    int i;
    for (i = 1; i < ANTEROOM_SIZE; i++) {
        hairdressers_antreroom->chairs[i - 1] = hairdressers_antreroom->chairs[i];
    }
    hairdressers_antreroom->chairs[ANTEROOM_SIZE - 1] = -1;
    sleep(1);
    hairdressers_antreroom->occupied--;
    printf("Barber: So %i -  you are ready! Take a look at mirror\n", cut_pid);
    kill(SIGKILL, cut_pid);
}

void prepare_shared_memory() {
    key_t project_key = ftok(shmPath, project_id);
    shared_memory_id = shmget(project_key, sizeof(struct antreroom), 0);
    if (shared_memory_id == -1) {
        printf("Barber: Opening shared memory failed!");
        return;
    }

    void *tmp = shmat(shared_memory_id, 0, 0);
    hairdressers_antreroom = (struct antreroom *) tmp;
}

void prepare_semaphores() {
    key_t project_key = ftok(shmPath, project_id);
    semaphore_id = semget(project_key, 0, 0);
    if (semaphore_id == -1) {
        printf("Barber: Couldn't create semaphore\n");
        return;
    }
}

int main(int argc, char *argv[]) {
    ANTEROOM_SIZE = atoi(argv[1]);
    prepare_semaphores();
    prepare_shared_memory();

    while (1) {
        get_semaphore(semaphore_id);
        if (hairdressers_antreroom->sleeping == 0) {
            if (check_antreroom_empty()) {
                go_to_sleep();
            } else {
                take_for_cutting();
            }
        }
        release_semaphore(semaphore_id);
    }
}
