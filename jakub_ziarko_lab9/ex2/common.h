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

#define BUFFER_SIZE 1000

/**
 * SEARCH_MODE:
 * 0 = equal L
 * 1 = greater than L
 * 2 = less than L
 *
 * OUTPUT_MODE:
 * 0 = normal
 * 1 = extendet
 */
struct buffer {
    clock_t begin;
    char products[1000][1000];
    int occupied;
    int index_manufacturer;
    int index_customer;
    int OUTPUT_MODE;
    int END_OF_FILE;
    int N;
    int K;
    int L;
    int P;
    int SEARCH_MODE;
    int nk;
    char input_path[1000];
};

const char *manufacturer_path = "/manufacturer_semaphore";
const char *customer_path = "/customer_semaphore";
const char *shmPath = "/shm";
sem_t *MANUFACTURER;
sem_t *CUSTOMER;
struct buffer *buf;

void print_shop() {
    printf("Occupied: %i\n", buf->occupied);
    int i;
    for (i = 0; i < buf->N; i++) {
        if (strcmp(buf->products[i],"") == 0)
            printf("%i) NULL\n", i);
        else
            printf("%i) %s\n", i, buf->products[i]);
    }
    printf("END\n\n");
}
