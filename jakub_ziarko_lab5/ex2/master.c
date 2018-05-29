#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

int main()
{
    int fd1;
    char str1[80];
    pid_t pid;

    srand(time(NULL));
    // FIFO file path
    char * myfifo = "/tmp/myfifo";
    char * pom = calloc(3, sizeof(char));
    // Creating the named file(FIFO)
    // mkfifo(<pathname>,<permission>)
    mkfifo(myfifo, 0666);

    while (1)
    {
        sprintf(pom, "%i", rand()%5);
        pid = fork();
        if (pid == 0){
            execl("./slave", "./slave", pom,  NULL);
            kill(getpid(), SIGKILL);
        } else {
            sleep(1);
            //First open in read only and read
            fd1 = open(myfifo, O_RDONLY);
            read(fd1, str1, 80);
            //Print the read string and close
            printf("Master: %s\n", str1);
            close(fd1);
            sleep(1);
        }
    }

    return 0;
}

