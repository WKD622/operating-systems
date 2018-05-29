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

pid_t barber_pid;

long get_timestamp() {
    struct timespec buffer;
    clock_gettime(CLOCK_MONOTONIC, &buffer);
    return buffer.tv_nsec / 1000;
}

const char *hairdressers_path = "/hairdressers_semaphore";
const char *shmPath = "/shm";

struct antreroom {
    int chairs[100];
    int occupied;
    int sleeping;
    int cutting;
};

struct antreroom *hairdressers_antreroom;
sem_t *HAIRDRESSERS;
