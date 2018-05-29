
/*
 * client.c: Client program
 *           to demonstrate interprocess commnuication
 *           with POSIX message queues
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define SERVER_QUEUE_NAME   "/sp-example-server"
#define QUEUE_PERMISSIONS 0660
#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

int main(int argc, char **argv) {
    char client_queue_name[200];
    mqd_t qd_server, qd_client;   // queue descriptors


    // create the client queue for receiving messages from server
    sprintf(client_queue_name, "/sp-example-client-%d", getpid());

    struct mq_attr attr;

    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((qd_client = mq_open(client_queue_name, O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        perror("Client: mq_open (client)");
        exit(1);
    }

    if ((qd_server = mq_open(SERVER_QUEUE_NAME, O_WRONLY)) == -1) {
        perror("Client: mq_open (server)");
        exit(1);
    }

    char in_buffer[MSG_BUFFER_SIZE];
    strcat(client_queue_name, " ");
    printf("Your message: ");

    char temp_buf[100];

    while (fgets(temp_buf, 100, stdin)) {
        char message[200];
        strcpy(message, client_queue_name);
        strcat(message, temp_buf);

        // send message to server
        if (mq_send(qd_server, message, strlen(message), 0) == -1) {
            perror("Client: Not able to send message to server");
            continue;
        }

        // receive response from server

        if (mq_receive(qd_client, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
            perror("Client: mq_receive");
            exit(1);
        }
        // display token received from server
        printf("Received from server: %s\n\n", in_buffer);

        printf("Your message: ");
    }


    if (mq_close(qd_client) == -1) {
        perror("Client: mq_close");
        exit(1);
    }

    if (mq_unlink(client_queue_name) == -1) {
        perror("Client: mq_unlink");
        exit(1);
    }
    printf("Client: bye\n");

    exit(0);
}
