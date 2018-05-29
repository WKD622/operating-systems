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
#include <sys/sem.h>
#include <sys/shm.h>

pid_t barber_pid;
const char *hairdressers_path = "/hairdressers_semaphore";
const char *shmPath = "/";
int shared_memory_id;
int semaphore_id;
int project_id = 123456789;

struct antreroom {
    int chairs[100];
    int occupied;
    int sleeping;
};

