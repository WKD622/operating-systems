#include "common.h"

int ANTEROOM_SIZE;

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
    printf("Barber: Let's cut %d.\n", cut_pid);
    hairdressers_antreroom->chairs[ANTEROOM_SIZE - 1] = -1;
    sleep(1);
    hairdressers_antreroom->occupied--;
    printf("Barber: So %i -  you are ready! Take a look at mirror\n", cut_pid);
    kill(SIGKILL, cut_pid);
}

void prepare_shared_memory() {
    int shmID = shm_open(shmPath, O_RDWR, 0666);
    if (shmID == -1) printf("Barber: Opening shared memory failed!");

    ftruncate(shmID, sizeof(struct antreroom));

    void *tmp = mmap(NULL, sizeof(struct antreroom), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
    hairdressers_antreroom = (struct antreroom *) tmp;
}

void prepare_semaphores() {
    HAIRDRESSERS = sem_open(hairdressers_path, O_RDWR);
    if (HAIRDRESSERS == SEM_FAILED) printf("Barber: Opening semaphore failed!");
}

void preapre_anteroom() {
    int i;
    for (i = 0; i < ANTEROOM_SIZE; i++)
        hairdressers_antreroom->chairs[i] = -1;
    hairdressers_antreroom->occupied = 0;
    hairdressers_antreroom->sleeping = 1;
}

int main(int argc, char *argv[]) {
    ANTEROOM_SIZE = atoi(argv[1]);
    prepare_semaphores();
    prepare_shared_memory();

    while (1) {
        sem_wait(HAIRDRESSERS);
        if (hairdressers_antreroom->sleeping == 0) {
            if (check_antreroom_empty()) {
                go_to_sleep();
            } else {
                take_for_cutting();
            }
        }
        sem_post(HAIRDRESSERS);
    }
}

