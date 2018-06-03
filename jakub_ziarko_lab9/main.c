#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#define BUFFER_SIZE 1000

//STRUCTURES
struct buffor {
    char **products;
    int occupied;
    int index_manufacturer;
    int index_customer;
    FILE *file;
};

// GLOBAL VARIABLES
struct buffor buf;
clock_t begin;
pthread_mutex_t mutex;
pthread_cond_t cond_c;
pthread_cond_t cond_m;
int END_OF_FILE = 0;
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
int P, K, N, L, OUTPUT_MODE, SEARCH_MODE, nk;


// FUNCTIONS AND METHODS
struct buffor init(char *path) {
    FILE *file;
    char *input_path = calloc(BUFFER_SIZE, sizeof(char));
    if ((file = fopen(path, "r")) == NULL) {
        printf("load_picture: Error during opening file.\n");
        return buf;
    }
    fscanf(file, "P=%d\n", &P);
    fscanf(file, "K=%d\n", &K);
    fscanf(file, "N=%d\n", &N);
    fscanf(file, "L=%d\n", &L);
    fscanf(file, "OUTPUT_MODE=%d\n", &OUTPUT_MODE);
    fscanf(file, "SEARCH_MODE=%d\n", &SEARCH_MODE);
    fscanf(file, "PATH=%s\n", input_path);
    fscanf(file, "nk=%d\n", &nk);
    struct buffor buf;
    buf.occupied = 0;
    buf.index_customer = 0;
    buf.index_manufacturer = 0;
    buf.products = calloc(N, sizeof(char *));
    if ((buf.file = fopen(input_path, "r")) == NULL) {
        printf("init_buffer: Error during opening file.\n");
        return buf;
    }
    return buf;
}

void end() {
    printf("\nENDING PROGRAM. GOODBYE!\n");
    exit(1);
}

void empty_array(char *array, int n) {
    int i;
    for (i = 0; i < n; i++)
        array[i] = 0;
}

void print_shop() {
    printf("Occupied: %i\n", buf.occupied);
    int i;
    for (i = 0; i < N; i++) {
        if (buf.products[i] == NULL)
            printf("%i) NULL\n", i);
        else
            printf("%i) %s\n", i, buf.products[i]);
    }
    printf("END\n\n");
}

char *read_next_line(FILE *file) {
    int value;
    char *line = calloc(1000, sizeof(char));
    empty_array(line, BUFFER_SIZE);
    char *ch = calloc(1, sizeof(char));
    fread(ch, sizeof(char), 1, file);
    while (strcmp(ch, "\n") != 0 && END_OF_FILE == 0) {
        strcat(line, ch);
        value = fread(ch, sizeof(char), 1, file);
        if (value == 0) {
            END_OF_FILE = 1;
        }
    }
    return line;
}

void create_new_product(FILE *file, int index) {
    buf.products[index] = read_next_line(file);
    if (OUTPUT_MODE == 1)
        printf("MANUFACTURER %ld: %s\n", pthread_self(), buf.products[index]);
    buf.occupied++;
}

void take_product(int index) {
    buf.occupied--;
    if (SEARCH_MODE == 0) {
        if (strlen(buf.products[index]) == L)
            printf("CUSTOMER     %ld: %s\n", pthread_self(), buf.products[index]);
    } else if (SEARCH_MODE == 1) {
        if (strlen(buf.products[index]) > L)
            printf("CUSTOMER     %ld: %s\n", pthread_self(), buf.products[index]);
    } else {
        if (strlen(buf.products[index]) < L)
            printf("CUSTOMER     %ld: %s\n", pthread_self(), buf.products[index]);
    }
    buf.products[index] = NULL;
}

bool check_if_empty() {
    if (buf.occupied == 0)
        return true;
    return false;
}

bool check_if_full() {
    if (buf.occupied == N)
        return true;
    return false;
}

void *customer(void *args) {
    while (1) {
        clock_t measure = clock();
        pthread_mutex_lock(&mutex);
        while (check_if_empty()) {
            pthread_cond_wait(&cond_c, &mutex);
        }
        if ((float) (measure - begin) / CLOCKS_PER_SEC < (float) nk) {
            int k = 0;
            if (buf.index_customer == N) {
                buf.index_customer = 0;
            }
            if (!check_if_full()) {
                k = 1;
            }
            take_product(buf.index_customer);
            if (END_OF_FILE == 1)
                end();
            buf.index_customer++;
            if (k == 1) {
                pthread_cond_broadcast(&cond_m);
            }
        } else {
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }
        sleep(1);
        pthread_mutex_unlock(&mutex);
    }
}

void *manufacturer(void *args) {
    while (1) {
        pthread_mutex_lock(&mutex);
        clock_t measure = clock();
        while (check_if_full()) {
            pthread_cond_wait(&cond_m, &mutex);
        }
        if ((float) (measure - begin) / CLOCKS_PER_SEC < (float) nk) {
            int k = 0;
            if (buf.index_manufacturer == N) {
                buf.index_manufacturer = 0;
            }
            if (!check_if_empty()) {
                k = 1;
            }
            create_new_product(buf.file, buf.index_manufacturer);
            buf.index_manufacturer++;
            if (k == 1) {
                pthread_cond_broadcast(&cond_c);
            }
        } else {
            pthread_mutex_unlock(&mutex);
            pthread_exit(NULL);
        }
        if (OUTPUT_MODE == 1)
            sleep(1);
        pthread_mutex_unlock(&mutex);
    }
}


// MAIN
int main() {
    signal(SIGINT, end);
    int i;
    begin = clock();
    char *path = "input/settings.txt";
    buf = init(path);
    mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    cond_c = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
    cond_m = (pthread_cond_t) PTHREAD_COND_INITIALIZER;

    pthread_t threads[P + K];
    for (i = 0; i < P; i++) {
        pthread_create(&threads[i], NULL, manufacturer, NULL);
    }
    for (i = P; i < K + P; i++) {
        pthread_create(&threads[i], NULL, customer, NULL);
    }
    for (int i = 0; i < K + P; i++)
        pthread_join(threads[i], NULL);
    return 0;
}
