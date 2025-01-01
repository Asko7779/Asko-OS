#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif

#define PORT 4444
#define MAX_CLIENTS 1

#ifdef _WIN32
void init_winsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        exit(1);
    }  }

void cleanup_winsock() {
    WSACleanup();
}
#endif

void handle_client(int client_sock) {
    char buffer[1024];
    int bytes_received;

    while (1) {
        memset(buffer, 0, sizeof(buffer));

        (command execution result)
        bytes_received = recv(client_sock, buffer, sizeof(buffer), 0);
        if (bytes_received <= 0) {
            break;
        }

        buffer[bytes_received] = '\0';
        printf("Client executed: %s\n", buffer);
    }
    close(client_sock);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len = sizeof(client_addr);

#ifdef _WIN32
    init_winsock();
#endif

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Server: Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Server: Bind failed");
        exit(1);
    }

    if (listen(server_sock, MAX_CLIENTS) == -1) {
        perror("Server: Listen failed");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_sock == -1) {
        perror("Server: Accept failed");
        exit(1);
    }

    printf("Client connected: %s\n", inet_ntoa(client_addr.sin_addr));
    handle_client(client_sock);

    close(server_sock);

#ifdef _WIN32
    cleanup_winsock();
#endif

    return 0;
}
