#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#define DEST_IP     "127.0.0.1"
#define DEST_PORT   3490

int main(int argc, char** argv) {

    // init WSA
    WSADATA wsaData;
    int errorHandler = 0;

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        return 1;
    }


    // define vars
    int sockfd;
    struct sockaddr_in my_addr;
    struct sockaddr_in dest_addr;


    // create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd == -1) {
        fprintf(stderr, "Error occured on creating sockfd.\n");
        return 1;
    }
    printf("[+]Client Socket is created.\n");


    // bind an ip & port
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = 0;           // kullanılmayan herhangi bir portu seç
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.2");
    memset(my_addr.sin_zero, '\0', sizeof(my_addr.sin_zero));

    errorHandler = bind(sockfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr));

    if(errorHandler == -1) {
        fprintf(stderr, "[-]Error occured on binding a port\n");
        return 1;
    }
    printf("[+]Binding a port has been successfully completed.\n");


    // connect a socket
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT);
    dest_addr.sin_addr.s_addr = inet_addr(DEST_IP);
    memset(dest_addr.sin_zero, '\0', sizeof(dest_addr.sin_zero));

    errorHandler = connect(sockfd, (struct sockaddr*)&dest_addr, sizeof(struct sockaddr));

    if(errorHandler == -1) {
        fprintf(stderr, "[-]Error occured on connecting: %s:%d\n", DEST_IP, DEST_PORT);
        return 1;
    } 
    printf("[+]Connected to Server.\n");

    // send messages to server.
    char buffer[1024];
    int bytes_sent;
    while(1) {
        printf("Client: ");
        scanf("%s", buffer);
        bytes_sent = send(sockfd, buffer, strlen(buffer), 0);

        if(bytes_sent == -1) {
            fprintf(stderr, "[-]Message couldn't send to server.\n");
            break;
        }

        // if(bytes_sent < strlen(buffer)) {}   // verinin tamamı gönderildi mi kontrolü.
    }

    closesocket(sockfd);
    printf("[+]Connection closed.");
    WSACleanup();
    return 0;
}