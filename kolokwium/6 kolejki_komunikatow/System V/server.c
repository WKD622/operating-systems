#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>


struct message { 
	char message[100];
	int client_msgqid;
};

int main(){
	int i;
	int server_msgqid;
	int client_msgqid;
	struct message incoming_message;
	
	// tworzę key dla messege queue servera
	int key = ftok("/home", 1);

	// tworze messege queue servera
	server_msgqid = msgget(key, 0600 | IPC_CREAT);

	//printfuje jego parametry
	printf("Server message queue (id: %i, key: %i)\n", server_msgqid, key);

	// odbieram wiadomosci od clientow i na nie odpisuje
	while(1){
		sleep(1);
		// na wszelki wypadek czyszcze miejsce na wiadomosc tekstowa
		for (i = 0; i < 100; i++)
			incoming_message.message[i] = 0;
		//odbieram wiadomość wysłaną przez klienta na messege queue servera, w tej wiadomości jest msgqid clienta do odpisania
		if (msgrcv(server_msgqid, &incoming_message, sizeof(incoming_message), 0, 0) == -1)
			perror("msgrcv");
		else { 
			// otwieram kolejke clienta
			msgget(incoming_message.client_msgqid, 0600 | IPC_CREAT);
			
			// odpisuje mu tą samą wiadomością
			msgsnd(incoming_message.client_msgqid, &incoming_message, sizeof(incoming_message), 0);
		}
	}
}

