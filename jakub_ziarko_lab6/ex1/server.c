#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MSGPERM 0600    // msg queue permission
#define MSGTXTLEN 128   // msg text length

/* int msgget(key_t key, int msgflg) - uzyskanie identyfikatora kolejki komunikatów używanego przez pozostałe funkcje,
 * int msgctl(int msgqid, int cmd, struct msqid_ds *buf) - ustawianie i pobieranie wartości parametrów związanych z kolejkami komunikatów oraz usuwanie kolejek,
 * msgsnd - wysłanie komunikatu,
 * msgrcv - odebranie komunikatu.
 * ftok - do generowania wartości kluczy.
 */

int msgqid, rc;

struct message {
    char *message;
    int key;
};
struct msg_buf {
    long mtype;
    char mtext[MSGTXTLEN];
} msg;

int getWords(char *base, char target[10][20]) {
    int n = 0, i, j = 0;

    for (i = 0; true; i++) {
        if (base[i] != ' ') {
            target[n][j++] = base[i];
        } else {
            target[n][j++] = '\0';//insert NULL
            n++;
            j = 0;
        }
        if (base[i] == '\0')
            break;
    }
    return n;

}

struct message *parseMessage(char *message, struct message *message1) {
    char *out = calloc(200, sizeof(char));
    char *pom = calloc(100, sizeof(char));
    time_t rawtime;
    struct tm *timeinfo;
    int n; //number of words
    int i; //loop counter
    char arr[100][20];
    n = getWords(message, arr);
    printf(" !%s! ", arr[1]);
    if (strcmp(arr[1], "MIRROR") == 0) {
        pom = arr[2];
        int len = (int) strlen(arr[1]);
        int j, k = 0;
        for (j = len - 1; j >= 0; j--) {
            out[k] = pom[j];
            k++;
        }
    } else if (strcmp(arr[1], "ADD") == 0) {
        int a = atoi(arr[2]);
        int b = atoi(arr[3]);
        int result = a + b;
        sprintf(out, "%i", result);
    } else if (strcmp(arr[1], "DIV") == 0) {
        int a = atoi(arr[2]);
        int b = atoi(arr[3]);
        int result = a / b;
        sprintf(out, "%i", result);
    } else if (strcmp(arr[1], "MUL") == 0) {
        int a = atoi(arr[2]);
        int b = atoi(arr[3]);
        int result = a * b;
        sprintf(out, "%i", result);
    } else if (strcmp(arr[1], "SUB") == 0) {
        int a = atoi(arr[2]);
        int b = atoi(arr[3]);
        int result = a - b;
        sprintf(out, "%i", result);
    } else if (strcmp(arr[1], "TIME\n") == 0) {
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        sprintf(out, "Current local time and date: %s", asctime(timeinfo));
    } else if (strcmp(arr[1], "END\n") == 0) {
        sprintf(out, "END");
    }

    message1->key = atoi(arr[0]);
    message1->message = calloc(100, sizeof(char));
    message1->message = out;
    return message1;
}

int main() {
    int key = ftok("/home/jakub/CLionProjects/lab6/ex1", 1);
    msgqid = msgget(key, MSGPERM | IPC_CREAT);
    printf("message queue %d, key: %i\n", msgqid, key);
    struct message *message1 = malloc(sizeof(struct message));
    while (1) {
        sleep(1);
        rc = msgrcv(msgqid, &msg, sizeof(msg.mtext), 0, 0);
        message1 = parseMessage(msg.mtext, message1);
        int rc_2 = msgget(message1->key, MSGPERM | IPC_CREAT);
        sprintf(msg.mtext, "%s\n", message1->message);
        rc_2 = msgsnd(message1->key, &msg, sizeof(msg.mtext), 0);
    }
}
