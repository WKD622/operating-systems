#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdbool.h>

#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 400
#define KEY_SIZE 100

struct message {
    char message[MAX_MSG_SIZE];
    char key[KEY_SIZE];
};


int main() {
	int i;
	struct message client_message;
	mqd_t qd_server, qd_client;
	
	struct mq_attr attr;	
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = 1000;
    attr.mq_curmsgs = 0;
	
	if ((qd_server = mq_open("/server-example", O_RDONLY | O_CREAT, 0644, &attr)) == -1) {
        perror("Server: mq_open");
        exit(1);
    }
	
	printf("Server message queue (id: %d, path: %s)\n", qd_server, "/server-example");

	while(1){
		for (i = 0; i < MAX_MSG_SIZE; i++)
			client_message.message[i] = 0;
	
		if (mq_receive(qd_server, (char *) &client_message, sizeof(client_message), NULL) == -1) {
            perror("Server: mq_receive");
            exit(1);
        } else {
			printf("message: %s\n", client_message.message);
			printf("key: %s\n", client_message.key);
			if ((qd_client = mq_open(client_message.key, O_WRONLY)) == -1) {
            	perror("Server: mq_open");
            	continue;
        	}	
			if (mq_send(qd_client, (const char *) &client_message, sizeof(client_message), 0) == -1) {
		        perror("Server: mq_send");
		        continue;
        	}
		}
	}
}
