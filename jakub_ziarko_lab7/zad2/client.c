#include "common.h"

int ANTEROOM_SIZE;

bool find_free_chair(pid_t pid) {
    printf("Client %i: Lets look for free chair in atreroom.\n", (int) pid);
    if (hairdressers_antreroom->occupied == ANTEROOM_SIZE) {
        printf("Client %i: No free chairs, I will come tomorrow!\n", pid);
        return false;
    }
    return true;
}

void occupie_chair(pid_t pid) {
    printf("%lo Client %i: Let's take a sit %d and wait for barber. \n", get_timestamp(), (int) pid,
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
    hairdressers_antreroom -> cutting = pid;
}

void prepare_shared_memory() {
    int shmID = shm_open(shmPath, O_RDWR, 0666);
    if (shmID == -1) printf("Client: Opening shared memory failed!");

    ftruncate(shmID, sizeof(struct antreroom));

    void *tmp = mmap(NULL, sizeof(struct antreroom), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
    hairdressers_antreroom = (struct antreroom *) tmp;
}

void prepare_semaphores() {
    HAIRDRESSERS = sem_open(hairdressers_path, O_RDWR);
    if (HAIRDRESSERS == SEM_FAILED) printf("Client: Opening  semaphore failed!");
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

    pid_t pid = getpid();
    printf("Client %d: I'm waiting for semaphore.\n", pid);
    sem_wait(HAIRDRESSERS);
    printf("Client %d: I got semaphore!\n", getpid());
    if (find_free_chair(pid)) {
        occupie_chair(pid);
        if (check_if_sleeping(pid)) {
            wake_up(pid);
        }
    }
    printf("Client %d: I released semaphore.\n", getpid());
    sem_post(HAIRDRESSERS);
}
