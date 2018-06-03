#include "common.h"

// GLOBAL VARIABLES
FILE *global_file;

// FUNCTIONS AND METHODS
void prepare_shared_memory() {
    int shmID = shm_open(shmPath, O_RDWR, 0666);
    if (shmID == -1)
        printf("manufacturer: Opening shared memory failed!");
    ftruncate(shmID, sizeof(struct buffer));
    void *tmp = mmap(NULL, sizeof(struct buffer), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
    buf = (struct buffer *) tmp;
    if ((global_file = fopen(buf->input_path, "r")) == NULL) {
        printf("manufacturer: Error during opening file.\n");
        return;
    }
}

void prepare_semaphores() {
    MANUFACTURER = sem_open(manufacturer_path, O_RDWR);
    if (MANUFACTURER == SEM_FAILED)
        printf("manufacturer: Opening  semaphore failed!");
    CUSTOMER = sem_open(customer_path, O_RDWR);
    if (CUSTOMER == SEM_FAILED)
        printf("manufacturer: Opening  semaphore failed!");
}

bool check_if_empty() {
    if (buf->occupied == 0)
        return true;
    return false;
}

bool check_if_full() {
    if (buf->occupied == buf->N)
        return true;
    return false;
}

void take_product(int index) {
    int i;
    buf->occupied--;
    if (buf->SEARCH_MODE == 0) {
        if (strlen(buf->products[index]) == buf->L)
            printf("CUSTOMER     %d: %s\n", getpid(), buf->products[index]);
    } else if (buf->SEARCH_MODE == 1) {
        if (strlen(buf->products[index]) > buf->L)
            printf("CUSTOMER     %d: %s\n", getpid(), buf->products[index]);
    } else {
        if (strlen(buf->products[index]) < buf->L)
            printf("CUSTOMER     %d: %s\n", getpid(), buf->products[index]);
    }
    for (i = 0; i < 1000; i++)
        buf->products[index][i] = 0;
}

// MAIN
int main() {
    prepare_shared_memory();
    prepare_semaphores();
    while (1) {
        sem_wait(CUSTOMER);
        if (!check_if_empty()) {
            clock_t measure = clock();
            if ((float) (measure - buf->begin) / CLOCKS_PER_SEC < (float) buf->nk) {
                if (buf->index_customer == buf->N)
                    buf->index_customer = 0;
                take_product(buf->index_customer);
                if (buf->END_OF_FILE == 1)
                    exit(1);
                buf->index_customer++;
            } else {
                exit(1);
            }
            sleep(1);
        }
        sem_post(CUSTOMER);
    }
}
