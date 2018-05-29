#include "common.h"

int ANTEROOM_SIZE, NUMBER_OF_CLIENTS;

struct antreroom *hairdressers_antreroom;

void prepare_shared_memory() {
    key_t project_key = ftok(shmPath, project_id);
    if (project_key == -1) {
        printf("Couldn't obtain a project key\n");
        return;
    }
    shared_memory_id = shmget(project_key, sizeof(struct antreroom), S_IRWXU | IPC_CREAT);
    if (shared_memory_id == -1) {
        printf("Couldn't create shared memory\n");
        return;
    }

    void *tmp = shmat(shared_memory_id, 0, 0);
    hairdressers_antreroom = (struct antreroom *) tmp;
}

void prepare_semaphores() {
    key_t project_key = ftok(shmPath, project_id);
    if (project_key == -1) {
        printf("Couldn't obtain a project key\n");
        return;
    }
    semaphore_id = semget(project_key, 1, IPC_CREAT | S_IRWXU);
    if (semaphore_id == -1) {
        printf("Couldn't create semaphore\n");
        return;
    }
    semctl(semaphore_id, 0, SETVAL, 1);
}

void preapre_anteroom() {
    int i;
    for (i = 0; i < ANTEROOM_SIZE; i++)
        hairdressers_antreroom->chairs[i] = -1;
    hairdressers_antreroom->occupied = 0;
    hairdressers_antreroom->sleeping = 1;
}

void at_exit() {
    kill(barber_pid, SIGKILL);
    printf("Barber: All cut? Let's go home!\n");
}

void prepare_numbers(int clients, int chairs) {
    NUMBER_OF_CLIENTS = clients;
    ANTEROOM_SIZE = chairs;
}

int main(int argc, char *argv[]) {
    int i;
    char ANTEROOM_SIZE_STRING[12];
    prepare_semaphores();
    prepare_shared_memory();
    preapre_anteroom();
    prepare_numbers(atoi(argv[1]), atoi(argv[2]));
    sprintf(ANTEROOM_SIZE_STRING, "%d", ANTEROOM_SIZE);
    signal(SIGINT, at_exit);

    fflush(stdout);

    barber_pid = fork();
    if (barber_pid == 0) {
        printf("Barber %d: I'm starting my work.\n", getpid());
        if (execl("./barber", "./barber", ANTEROOM_SIZE_STRING, NULL) < 0);
        printf("sth went wrong (BARBER)\n");
        exit(1);
    }
    for (i = 0; i < 10; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            execl("./client", "./client", ANTEROOM_SIZE_STRING, NULL);
            printf("sth went wrong (CLIENT)\n");
            exit(1);
        }
    }

    for (i = 0; i < NUMBER_OF_CLIENTS + 1; i++) {
        int status;
        printf("EXITED %d \n", i);
        wait(&status);
    }
    at_exit();

    return 0;
}
