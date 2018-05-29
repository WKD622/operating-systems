#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdbool.h>
#include <time.h>

#define SERVER_QUEUE_NAME   "/sp-example-server"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

struct message {
    char *message;
    char *key;
};

int getWords(char *base, char target[100][100]) {
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

struct message *parseMessage(char *message, struct message *message1, mqd_t qd_client) {
    char *out = calloc(200, sizeof(char));
    char *pom = calloc(100, sizeof(char));
    time_t rawtime;
    struct tm *timeinfo;
    int n; //number of words
    int i; //loop counter
    char arr[100][100];
    n = getWords(message, arr);
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
        if (mq_close (qd_client) == -1) {
            perror ("Client: mq_close");
            exit (1);
        }
        printf ("Client: bye\n");
    }

    message1->message = calloc(100, sizeof(char));
    message1->key = calloc(100, sizeof(char));
    message1->key = arr[0];
    message1->message = out;
    return message1;
}

int main(int argc, char **argv) {
    mqd_t qd_server, qd_client;   // queue descriptors

    printf("Server: Hi!\n");

    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((qd_server = mq_open(SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror("Server: mq_open (server)");
        exit(1);
    }
    char in_buffer[MSG_BUFFER_SIZE];
    char out_buffer[MSG_BUFFER_SIZE];
    int i;
    while (1) {
        for (i = 0; i < 100; i++)
            in_buffer[i] = 0;

        // get the oldest message with highest priority
        if (mq_receive(qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror("Server: mq_receive");
            exit(1);
        }


        printf("Server: message received.\n");
        struct message *message1 = calloc(1, sizeof(struct message));
        message1 = parseMessage(in_buffer, message1, qd_client);

        // send reply message to client
        if ((qd_client = mq_open(message1->key, O_WRONLY)) == 1) {
            perror("Server: Not able to open client queue");
            continue;
        }

        sprintf(out_buffer, "%s\n", message1->message);

        if (mq_send(qd_client, out_buffer, strlen(out_buffer), 0) == -1) {
            perror("Server: Not able to send message to client");
            continue;
        }

        printf("Server: response sent to client.\n");
    }
}
