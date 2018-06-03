#include "common.h"

//GLOBAL VARIABLES
int END_OF_FILE = 0;
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

void empty_array(char *array, int n) {
    int i;
    for (i = 0; i < n; i++)
        array[i] = 0;
}

char *read_next_line(FILE *file, char *line) {
    int value = 0;
    empty_array(line, BUFFER_SIZE);
    char *ch = calloc(1, sizeof(char));
    value = fread(ch, sizeof(char), 1, file);
    while (strcmp(ch, "\n") != 0 && buf->END_OF_FILE == 0) {
        strcat(line, ch);
        value = fread(ch, sizeof(char), 1, file);
        if (value == 0) {
            buf->END_OF_FILE = 1;
        }
    }
    return line;
}

void create_new_product(FILE *file, int index) {
    char *line = calloc(1000, sizeof(char));
    line = read_next_line(file, line);
    strcpy(buf->products[index], line);
    if (buf->OUTPUT_MODE == 1)
        printf("MANUFACTURER %d: %s\n", getpid(), buf->products[index]);
    buf->occupied++;
}

// MAIN
int main() {
    prepare_shared_memory();
    prepare_semaphores();
    while (1) {
        sem_wait(MANUFACTURER);
        if (!check_if_full()) {
            clock_t measure = clock();
            if ((float) (measure - buf->begin) / CLOCKS_PER_SEC < (float) buf->nk) {
                if (buf->index_manufacturer == buf->N)
                    buf->index_manufacturer = 0;
                create_new_product(global_file, buf->index_manufacturer);
                buf->index_manufacturer++;
            } else {
                exit(1);
            }
            if (buf->OUTPUT_MODE == 1)
                sleep(1);
        }
        if (buf->END_OF_FILE == 1)
            exit(1);
        sem_post(MANUFACTURER);
    }
}
