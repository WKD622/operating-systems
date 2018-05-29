#include "common.h"

int ANTEROOM_SIZE, NUMBER_OF_CLIENTS;

void prepare_shared_memory() {
    int shmID = shm_open(shmPath, O_RDWR | O_CREAT, 0666);
    if (shmID == -1) printf("Opening shared memory failed!");
    ftruncate(shmID, sizeof(struct antreroom));
    void *tmp = mmap(NULL, sizeof(struct antreroom), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
    hairdressers_antreroom = (struct antreroom *) tmp;
}

void prepare_semaphores() {
    HAIRDRESSERS = sem_open(hairdressers_path, O_RDWR | O_CREAT | O_EXCL, 0777, 1);
    if (HAIRDRESSERS == SEM_FAILED) {
        sem_unlink(hairdressers_path);
        HAIRDRESSERS = sem_open(hairdressers_path, O_RDWR | O_CREAT | O_EXCL, 0777, 1);
        if (HAIRDRESSERS == SEM_FAILED) {
            printf("Couldn't create semaphore!\n");
        }
    }
}

void preapre_anteroom() {
    int i;
    for (i = 0; i < ANTEROOM_SIZE; i++)
        hairdressers_antreroom->chairs[i] = -1;
    hairdressers_antreroom->occupied = 0;
    hairdressers_antreroom->sleeping = 1;
}

void close_semaphores() {
    if (sem_close(HAIRDRESSERS) == -1) printf("Closing semaphore failed!\n");
}

void at_exit() {
    kill(barber_pid, SIGKILL);
    printf("Barber: All cut? Let's go home!\n");
    close_semaphores();
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
        if (execl("./barber", "./barber", ANTEROOM_SIZE_STRING, NULL) < 0);
        perror("error\n");
        printf("Something went wrong while execing barber\n");
        exit(1);
    }
    printf("Barber: I'm starting my work.\n");
    for (i = 0; i < NUMBER_OF_CLIENTS; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            execl("./client", "./client", ANTEROOM_SIZE_STRING, NULL);
            printf("Something went wrong while execing client\n");
            exit(1);
        }
    }

    for (i = 0; i < NUMBER_OF_CLIENTS + 1; i++) {
        int status;
        wait(&status);
    }

    at_exit();

    return 0;
}

