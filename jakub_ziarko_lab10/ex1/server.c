#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define MAX_NUMBER_OF_CLIENTS 20
int index_to_take;


// STRUCTURES
struct client {
    char name[100];
    int socket;
};

/**
 * 0 - dodawanie
 * 1 - odejmowanie
 * 2 - mnożenie
 * 3 - dzielenie
 */
struct to_send {
    int val1;
    int val2;
    int operation;
};


// GLOBAL VARIABLES
struct client client_sockets[MAX_NUMBER_OF_CLIENTS];
int occupied, socket_desc, i, sock_to_send, result, accepting_clients;
struct sockaddr_in server;
struct to_send to_send1;
pthread_t accepting_clients_thread;


// FUNCTIONS AND METHODS
void prepare_array() {
    int i, j;
    for (i = 0; i < MAX_NUMBER_OF_CLIENTS; i++) {
        client_sockets[i].socket = 0;
        for (j = 0; j < 100; j++)
            client_sockets[i].name[j] = 0;
    }
}

void prepare_global_variables() {
    occupied = 0;
    index_to_take = 0;
    accepting_clients = 0;
}

void prepare_server() {
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        puts("Server: Could not create socket.");
    }
    puts("Server: Socket created");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    if (bind(socket_desc, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("Server: Bind failed. Error.");
        return;
    }
    puts("Server: Bind done.");
}

void print_clients() {
    int i;
    for (i = 0; i < MAX_NUMBER_OF_CLIENTS; i++) {
        printf("%i  ", client_sockets[i].socket);
    }
    printf("\n");
    for (i = 0; i < MAX_NUMBER_OF_CLIENTS; i++) {
        printf("%s  ", client_sockets[i].name);
    }
    printf("\n");
}

void print_to_send() {
    printf("val1: %d\n", to_send1.val1);
    printf("val2: %d\n", to_send1.val2);
    printf("operation: %d\n", to_send1.operation);
}

void remove_(int socket) {
    int i, j;
    for (i = 0; i < MAX_NUMBER_OF_CLIENTS; i++) {
        if (client_sockets[i].socket == socket) {
            client_sockets[i].socket = 0;
            for (j = 0; j < 100; j++)
                client_sockets[i].name[j] = 0;
            return;
        }
    }
}

void add(int socket, char *name) {
    int i;
    while (i < MAX_NUMBER_OF_CLIENTS && client_sockets[i].socket != 0)
        i++;
    client_sockets[i].socket = socket;
    strcpy(client_sockets[i].name, name);
}

void *accept_connection(void *arg) {
    int socket_desc = *(int *) arg;
    listen(socket_desc, MAX_NUMBER_OF_CLIENTS);
    int c = sizeof(struct sockaddr_in);
    char client_message[1000];
    int client_sock;
    struct sockaddr_in client;
    while ((client_sock = accept(socket_desc, (struct sockaddr *) &client, (socklen_t * ) & c))) {
        printf("%i\n", client_sock);
        struct timeval tv;
        tv.tv_sec = 10;
        tv.tv_usec = 0;
        setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv, sizeof tv);
        puts("Server: Connection accepted!\n");
        occupied++;

        if (recv(client_sock, client_message, 1000, 0) > 0) {
            add(client_sock, client_message);
            puts("Server: Name received.\n");
        } else {
            perror("Server: No name received.\n");
        }
    }
    if (client_sock < 0) {
        perror("Server: Accept failed.");
    }
    return NULL;
}

void turn_on_accepting_clients() {
    accepting_clients = 1;
    pthread_create(&accepting_clients_thread, NULL, accept_connection, &socket_desc);
}

int operation_to_number(char c) {
    if (c == '+') {
        return 0;
    } else if (c == '-') {
        return 1;
    } else if (c == '*') {
        return 2;
    } else {
        return 3;
    }
}

struct to_send fill_message_structure() {
    int arg1, arg2;
    char op;
    struct to_send to_send1;
    puts("Server: Enter message:");
    scanf("%d %c %d", &arg1, &op, &arg2);
    to_send1.val1 = arg1;
    to_send1.val2 = arg2;
    to_send1.operation = operation_to_number(op);
    return to_send1;
}

void wait_until_any_client() {
    while (occupied == 0) {
        sleep(1);
    }
}

void choose_socket_to_send_to() {
    sock_to_send = 0;
    while (sock_to_send == 0) {
        sock_to_send = client_sockets[rand() % MAX_NUMBER_OF_CLIENTS].socket;
    }
}


// MAIN
int main(int argc, char *argv[]) {
    srand(time(NULL));
    prepare_array();
    prepare_global_variables();
    prepare_server();
    turn_on_accepting_clients();

    while (1) {
        wait_until_any_client();
        choose_socket_to_send_to();
        to_send1 = fill_message_structure();
        print_to_send();
        if (send(sock_to_send, &to_send1, sizeof(to_send1), 0) < 0) {
            puts("Server: Send failed.");
            return 1;
        }
        sleep(1);
        if (recv(sock_to_send, &result, sizeof(result), 0) < 0) {
            puts("Server: Receive failed.");
            break;
        } else {
            printf("Server: Result is %d\n", result);
        }
    }
}
