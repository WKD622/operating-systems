#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <stdio.h>
#include <unistd.h>


struct message { 
	char message[100];
	int client_msgqid;
};

int main(){
	int i;
	struct message message_to_send;
	struct message incoming_message;
	
	// tworzę key dla messege queue servera
	int key_server = ftok("/home", 1);

	// tworze messege queue servera
	int server_msgqid = msgget(key_server, 0600 | IPC_CREAT);
	printf("Server message queue (id: %i, key: %i)\n", server_msgqid, key_server);

	// tworzę key dla messege queue clienta
	int key_client = ftok("/home", 2);

	// tworze messege queue clienta
	int client_msgqid = msgget(key_client, 0600 | IPC_CREAT);
	printf("Client message queue (id: %i, key: %i)\n", client_msgqid, key_client);
	
	// wysylam wiadomosci do servera i odbieram odpowiedzi od servera
	int pid = fork();
	if (pid == 0) {
		// wysylanie wiadomosci
	    while (1) {
			// na wszelki wypadek czyszcze miejsce na wiadomosc tekstowa
			for (i = 0; i < 100; i++)
				message_to_send.message[i] = 0;
	        message_to_send.client_msgqid = client_msgqid;
			printf("YOUR MESSAGE: \n");
	        fgets(message_to_send.message, 100, stdin);
	        msgsnd(server_msgqid, &message_to_send, sizeof(message_to_send), 0);
	    }
	} else {
		// odbieranie wiadomosci
	    while (1) {
	        sleep(1);
	        msgrcv(client_msgqid, &incoming_message, sizeof(incoming_message), 0, 0);
	        printf("\nRECEIVED: %s\n", incoming_message.message);
	    }
	}
}
