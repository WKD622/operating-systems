#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <zconf.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

char *date_and_pid_string(char *to_write, struct tm *timeinfo) {
    sprintf(to_write, "PID: %i    |   TIME: %s", getpid(), asctime(timeinfo));
    return to_write;
}

int main(int argc, char *argv[])
{
    int fd, i, N = atoi(argv[1]);
    char * myfifo = "/tmp/myfifo";
    mkfifo(myfifo, 0666);
    time_t rawtime;
    struct tm *timeinfo;
    char *to_write = calloc(100, sizeof(char));
    char arr1[80];

    for (i = 0; i < N; i++)
    {
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        fd = open(myfifo, O_WRONLY);
        to_write = date_and_pid_string(to_write, timeinfo);
        write(fd, to_write, strlen(to_write)+1);
        close(fd);
        sleep(1);
    }
    return 0;
}
