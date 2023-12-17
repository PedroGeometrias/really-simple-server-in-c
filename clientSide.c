#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1" 
#define PORT 8080
#define BUFF_SIZE 1024

// Function declarations
int createSocket();
void setupServerAddress(struct sockaddr_in *address, const char *ip, int port);
int connectToServer(int sock, const struct sockaddr_in *address);
void sendData(int sock, const char *message);
void receiveData(int sock, char *buffer, size_t bufferSize);

int main() {
    printf("Initializing client...\n");
    int sock = createSocket();

    struct sockaddr_in address;
    setupServerAddress(&address, SERVER_IP, PORT);

    printf("Attempting to connect to server at %s:%d...\n", SERVER_IP, PORT);
    connectToServer(sock, &address);
    printf("Connected to server. You can now send messages.\n");

    while (1) {
        char message[BUFF_SIZE];
        printf("Enter a message (or type 'exit' to quit): ");
        fgets(message, BUFF_SIZE, stdin);

        // Trim newline character
        size_t len = strlen(message);
        if (len > 0 && message[len - 1] == '\n') {
            message[len - 1] = '\0';
        }

        // Exit condition
        if (strcmp(message, "exit") == 0) {
            break;
        }

        sendData(sock, message);

        char buffer[BUFF_SIZE];
        receiveData(sock, buffer, sizeof(buffer));
        printf("Server says: %s\n", buffer);
    }

    printf("Exiting client application...\n");
    close(sock);
    return 0;
}

// Function to create a socket
int createSocket() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    return sock;
}

// Function to set up server address structure
void setupServerAddress(struct sockaddr_in *address, const char *ip, int port) {
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &address->sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }
}

// Function to establish a connection to the server
int connectToServer(int sock, const struct sockaddr_in *address) {
    if (connect(sock, (struct sockaddr *)address, sizeof(*address)) == -1) {
        perror("connect");
        close(sock);
        exit(EXIT_FAILURE);
    }
    return 0;
}

// Function to send data to the server
void sendData(int sock, const char *message) {
    if (send(sock, message, strlen(message), 0) == -1) {
        perror("send");
        close(sock);
        exit(EXIT_FAILURE);
    }
}

// Function to receive data from the server
void receiveData(int sock, char *buffer, size_t bufferSize) {
    ssize_t bytesReceived = recv(sock, buffer, bufferSize - 1, 0);
    if (bytesReceived == -1) {
        perror("recv");
        close(sock);
        exit(EXIT_FAILURE);
    }
    buffer[bytesReceived] = '\0'; // Null-terminate the received data
}

