#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>


// STRUCTURES
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
int sock, i, result;
struct sockaddr_in server;
char message[1000];
struct to_send receive;


// FUNCTIONS AND METHODS
int solve(int op, int arg1, int arg2) {
    if (op == 0) {
        return arg1 + arg2;
    } else if (op == 1) {
        return arg1 - arg2;
    } else if (op == 2) {
        return arg1 * arg2;
    } else {
        return arg1 / arg2;
    }
}

void prepare_server() {
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);

    if (connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        perror("connect failed. Error");
        return;
    }
    puts("Connected\n");
}

void empty_struct() {
    receive.operation = 0;
    receive.val1 = 0;
    receive.val2 = 0;
}

void first_message() {
    printf("Enter your name, you have limited time.\n");
    scanf("%s", message);
    if (send(sock, message, 1000, 0) < 0) {
        puts("Send failed");
        return;
    }
}

void print_receive() {
    printf("val1: %d\n", receive.val1);
    printf("val2: %d\n", receive.val2);
    printf("operation: %d\n", receive.operation);
}


// MAIN
int main(int argc, char *argv[]) {
    prepare_server();
    first_message();

    while (1) {
        empty_struct();
        if (recv(sock, &receive, sizeof(receive), 0) < 0) {
            puts("Client: Receive failed");
            break;
        } else {
            print_receive();
            result = solve(receive.operation, receive.val1, receive.val2);
            printf("po obliczeniach %d\n", result);
            if (send(sock, &result, sizeof(result), 0) < 0) {
                puts("Client: Send failed");
                return 1;
            }
        }
    }

    //close(sock);
    return 0;
}