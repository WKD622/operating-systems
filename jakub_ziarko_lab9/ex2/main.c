#include "common.h"

// GLOBAL VARIABLES
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
int P_, K_, N_, L_, OUTPUT_MODE_, SEARCH_MODE_, nk_;
char *input_path;
pid_t children[1000];

// FUNCTIONS AND METHODS
void prepare_shared_memory() {
    int shmID = shm_open(shmPath, O_RDWR | O_CREAT, 0666);
    if (shmID == -1)
        printf("Opening shared memory failed!");
    ftruncate(shmID, sizeof(struct buffer));
    void *tmp = mmap(NULL, sizeof(struct buffer), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
    buf = (struct buffer *) tmp;
}

void prepare_buffer() {
    int i, j;
    for (i = 0; i < 1000; i++) {
        buf->input_path[i] = 0;
        for (j = 0; j < 1000; j++)
            buf->products[i][j] = 0;
    }

    buf->occupied = 0;
    buf->index_customer = 0;
    buf->index_manufacturer = 0;
    buf->begin = clock();
    buf->OUTPUT_MODE = OUTPUT_MODE_;
    buf->L = L_;
    buf->N = N_;
    buf->K = K_;
    buf->P = P_;
    buf->SEARCH_MODE = SEARCH_MODE_;
    buf->nk = nk_;
    buf->END_OF_FILE = 0;
    strcpy(buf->input_path, input_path);
}

void prepare_semaphores() {
    MANUFACTURER = sem_open(manufacturer_path, O_RDWR | O_CREAT | O_EXCL, 0777, 1);
    if (MANUFACTURER == SEM_FAILED) {
        sem_unlink(manufacturer_path);
        MANUFACTURER = sem_open(manufacturer_path, O_RDWR | O_CREAT | O_EXCL, 0777, 1);
        if (MANUFACTURER == SEM_FAILED) {
            printf("Couldn't create semaphore MANUFACTURER!\n");
        }
    }
    CUSTOMER = sem_open(customer_path, O_RDWR | O_CREAT | O_EXCL, 0777, 1);
    if (CUSTOMER == SEM_FAILED) {
        sem_unlink(customer_path);
        CUSTOMER = sem_open(customer_path, O_RDWR | O_CREAT | O_EXCL, 0777, 1);
        if (CUSTOMER == SEM_FAILED) {
            printf("Couldn't create semaphore CUSTOMER!\n");
        }
    }
}

void init_variables(char *path) {
    FILE *file;
    input_path = calloc(BUFFER_SIZE, sizeof(char));
    if ((file = fopen(path, "r")) == NULL) {
        printf("load_picture: Error during opening file.\n");
        return;
    }
    fscanf(file, "P=%d\n", &P_);
    fscanf(file, "K=%d\n", &K_);
    fscanf(file, "N=%d\n", &N_);
    fscanf(file, "L=%d\n", &L_);
    fscanf(file, "OUTPUT_MODE=%d\n", &OUTPUT_MODE_);
    fscanf(file, "SEARCH_MODE=%d\n", &SEARCH_MODE_);
    fscanf(file, "PATH=%s\n", input_path);
    fscanf(file, "nk=%d\n", &nk_);
}

void end() {
    int i;
    for (i = 0; i < P_ + K_; i++)
        kill(children[i], SIGKILL);
    printf("\nENDING PROGRAM. GOODBYE!\n");
    exit(1);
}

// MAIN
int main() {
    int i;
    char *path = "input/settings.txt";
    signal(SIGINT, end);
    init_variables(path);
    prepare_shared_memory();
    prepare_semaphores();
    prepare_buffer();
    for (i = 0; i < P_; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            if (execl("./manufacturer", "./manufacturer", NULL) == -1) {
                printf("Something went wrong while running customer\n");
                end();
            }
        } else {
            children[i] = pid;
        }
    }
    for (i = P_; i < P_ + K_; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            if (execl("./customer", "./customer", NULL) == -1) {
                printf("Something went wrong while running manufacturer\n");
                end();
            }
        } else {
            children[i] = pid;
        }
    }
    for (int i = 0; i < P_ + K_; i++) {
        int status;
        wait(&status);
    }
    sleep(10);
}
