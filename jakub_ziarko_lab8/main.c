#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/resource.h>
#include <time.h>

int WIDTH, HEIGHT, MAX_VALUE, P, C, number_of_threads;
int **I;
int **J;
float **K;

struct timeMeasurment{
    float kernelTime;
    float userTime;
};

float sumMicroSecAndSec(int sec, int microSec){
    return (float) microSec / 1000000 + (float) sec;
}

int ceil_(float f) {
    int i = 0;
    while (i < f) {
        i++;
    }
    return i;
}

int round_(float f) {
    int i = 0;
    while (i < f) {
        i++;
    }
    i--;
    f = f - (float) i;
    if (f >= 0.5)
        return i + 1;
    else
        return i;
}

int max(int a, int b) {
    if (a > b) return a;
    return b;
}

void print_filter(float **filter) {
    int i, j;
    for (i = 0; i < C; i++) {
        for (j = 0; j < C; j++) {
            printf("%f ", filter[i][j]);
        }
        printf("\n");
    }
}

void print_picture(int **picture, int width, int height) {
    int i, j;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            printf("%i ", picture[i][j]);
        }
        printf("\n");
    }
}

int **load_picture(char *path) {
    FILE *file;
    int i, j;
    if ((file = fopen(path, "r")) == NULL) {
        printf("load_picture: Error during opening file.\n");
        return NULL;
    }
    fscanf(file, "P%d\n", &P);
    fscanf(file, "%d", &WIDTH);
    fscanf(file, "%d", &HEIGHT);
    fscanf(file, "%d", &MAX_VALUE);

    int **picture = (int **) calloc((size_t) WIDTH, sizeof(int));
    for (i = 0; i < WIDTH; i++)
        picture[i] = calloc((size_t) HEIGHT, sizeof(int));

    for (i = 0; i < HEIGHT; i++)
        for (j = 0; j < WIDTH; j++)
            fscanf(file, "%d", &picture[i][j]);

    fclose(file);
    return picture;
}

float **load_filter(char *path) {
    FILE *file;
    int i, j;
    if ((file = fopen(path, "r")) == NULL) {
        printf("load_filter: Error during opening file.\n");
        return NULL;
    }

    fscanf(file, "%d", &C);

    float **filter = (float **) calloc((size_t) C, sizeof(float));
    for (i = 0; i < C; i++)
        filter[i] = calloc((size_t) C, sizeof(int));

    for (i = 0; i < C; i++)
        for (j = 0; j < C; j++)
            fscanf(file, "%f", &filter[i][j]);

    fclose(file);
    return filter;
}

int **alloc_out_picture(int **out_picture) {
    int i;
    out_picture = (int **) calloc((size_t) WIDTH, sizeof(int));
    for (i = 0; i < WIDTH; i++)
        out_picture[i] = calloc((size_t) HEIGHT, sizeof(int));
    return out_picture;
}

void save_picture(int **out_picture, char *path) {
    FILE *file;
    int i, j;
    if ((file = fopen(path, "w")) == NULL) {
        printf("save_picture: Error during opening file.\n");
        return;
    }
    fprintf(file, "P%d\n%d %d\n%d\n", 2, WIDTH, HEIGHT, MAX_VALUE);
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            fprintf(file, "%d ", out_picture[i][j]);
            if (j == WIDTH - 1) fprintf(file, "\n");
        }
    }
    fclose(file);
}

void *routine(void *args) {
    int x, y, i = 0, j = 0;
    float sum;
    int thread_number = *(int *) args;

    int start = (WIDTH * HEIGHT * (thread_number - 1)) / number_of_threads;
    // if (start == 0) start++;
    int end = (WIDTH * HEIGHT * thread_number) / number_of_threads;
    //printf("start: %i  end: %i\n", start, end);
    int start_y = start / number_of_threads;
    int end_y = end / number_of_threads;
    int start_x = start % number_of_threads;
    int end_x = end % number_of_threads;
    int start_x_copy = start_x;
    int end_x_copy = end_x;
    //printf("start_y: %i  end_y: %i\n", start_y, end_y);
    //printf("start_x: %i  end_x: %i\n\n\n", start_x, end_x);
    for (y = start_y; y <= end_y; y++) {
        if (y == start_y) {
            start_x = start_x_copy;
            end_x = WIDTH - 1;
        } else if (y == end_y) {
            start_x = 0;
            end_x = end_x_copy;
        } else {
            start_x = 0;
            end_x = WIDTH - 1;
        }
        for (x = start_x; x <= end_x; x++) {
            //printf("x: %i   y: %i\n", x, y);
            sum = 0.0;
            for (i = 0; i < C; i++) {
                for (j = 0; j < C; j++) {
                    int w = max(0, x - ceil_((float) (C / 2.0)) + i - 1);
                    int h = max(0, y - ceil_((float) (C / 2.0)) + j - 1);
                    sum += I[w][h] * K[i][j];
                }
            }
            J[x][y] = round_(sum);
        }
    }
    return NULL;
}

void measureTime(struct timeMeasurment * mes, char * message){
    struct timeval user;
    struct timeval kernel;
    struct rusage rusage1;
    float secKernel, secUser;

    getrusage(RUSAGE_SELF, &rusage1);
    user = rusage1.ru_utime;
    kernel = rusage1.ru_stime;
    secKernel = sumMicroSecAndSec(kernel.tv_sec, kernel.tv_usec);
    secUser = sumMicroSecAndSec(user.tv_sec, user.tv_usec);
    printf("%s kernel: %fs   |   user: %fs\n\n", message, secKernel - mes->kernelTime, secUser - mes->userTime);
    mes->kernelTime = secKernel;
    mes->userTime = secUser;
}

int main(int argc, char *argv[]) {
    struct timeMeasurment mes;
    struct timeval user;
    struct timeval kernel;
    struct rusage rusage1;
    float startUserTime, startKernelTime;

    int i;
    number_of_threads = atoi(argv[1]);
    char *picture_path = argv[2];
    char *filter_path = argv[3];
    char *out_path = argv[4];
    I = load_picture(picture_path);
    K = load_filter(filter_path);
    J = alloc_out_picture(J);
    pthread_t *threads = calloc((size_t) number_of_threads, sizeof(pthread_t));

    getrusage(RUSAGE_SELF, &rusage1);
    user = rusage1.ru_utime;
    kernel = rusage1.ru_stime;
    mes.kernelTime = sumMicroSecAndSec(kernel.tv_sec, kernel.tv_usec);
    mes.userTime = sumMicroSecAndSec(user.tv_sec, user.tv_usec);
    startKernelTime = mes.kernelTime;
    startUserTime = mes.userTime;

    for (i = 1; i <= number_of_threads; i++) {
        int *arg = malloc(sizeof(int));
        *arg = i;
        pthread_create(&threads[i], NULL, routine, arg);
    }
    
    measureTime(&mes, "AFTER ALL OPERATIONS");
    save_picture(J, out_path);

}
