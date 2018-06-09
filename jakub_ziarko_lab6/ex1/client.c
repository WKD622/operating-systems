#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <stdio.h>

#define MSGPERM 0600    // msg queue permission
#define MSGTXTLEN 128   // msg text length

/* int msgget(key_t key, int msgflg) - uzyskanie identyfikatora kolejki komunikatów używanego przez pozostałe funkcje,
 * int msgctl(int msgqid, int cmd, struct msqid_ds *buf) - ustawianie i pobieranie wartości parametrów związanych z kolejkami komunikatów oraz usuwanie kolejek,
 * msgsnd - wysłanie komunikatu,
 * msgrcv - odebranie komunikatu.
 * ftok - do generowania wartości kluczy.
 */

int msgqid_1, msgqid_2, rc;

struct msg_buf {
    long mtype;
    char mtext[MSGTXTLEN];
} msg;

int main() {
    char message[100];
    char message2[100];
    msg.mtype = 1;  // set the type of message

    int key_1 = ftok("/home/jakub/CLionProjects/lab6/ex1", 1);
    msgqid_1 = msgget(key_1, MSGPERM | IPC_CREAT);
    printf("public message queue %d, key: %i\n", msgqid_1, key_1);

    int key_2 = ftok("/home/jakub/CLionProjects/lab6/ex1", 2);
    msgqid_2 = msgget(key_2, MSGPERM | IPC_CREAT);
    printf("private message queue %d, key: %i\n", msgqid_2, key_2);


    int pid = fork();
    if (pid == 0) {
        while (1) {
            printf("YOUR MESSAGE: \n");
            sprintf(message2, "%i ", msgqid_2);
            fgets(message, 100, stdin);
            strcat(message2, message);
            sprintf(msg.mtext, "%s", message2);
            printf("%s", msg.mtext);
            rc = msgsnd(msgqid_1, &msg, sizeof(msg.mtext), 0);
        }
    } else {
        while (1) {
            sleep(1);
            rc = msgrcv(msgqid_2, &msg, sizeof(msg.mtext), 0, 0);
            printf("\nRECEIVED: %s\n", msg.mtext);
        }
    }
}
