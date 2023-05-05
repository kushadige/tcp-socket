#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#define MYPORT 3490    // kullanıcıların bağlanacağı port
#define BACKLOG 10     // kuyrukta bekleyecek bağlantı sayısı

int main(int argc, char** argv) {

    WSADATA wsaData;
    int errorHandler = 0;

    // initialize WSAStartup function which is inside the Ws2_32.lib the network programming on windows.
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
        return 1;
    }

    // defining variables for tcp connection
    int sockfd, new_fd;                 // sockfd ile dinle, new_fd yeni bağlantı için
     // new_fd şeklindeki soket tanımlayıcıyı tüm send() ve recv() işlevleri için kullanacağız. Eğer sadece ve sadece tek bir bağlantıyı kabul edecekseniz close() _> (LINUX) - closesocket() _> (WINDOWS) ile sockfd üzerindeki dinleyici soketi kapatabilirsiniz böylece aynı port üzerinden başka bağlantı yaplamaz.

    struct sockaddr_in my_addr;         // benim adres bilgim
    struct sockaddr_in their_addr;      // bağlananın adres bilgisi
    int sin_size;
    

    // creating socket 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd == -1) {
        fprintf(stderr, "Error occured on creating sockfd.\n");
        return 1;
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(MYPORT);                      // host to network short translation
    my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");    // inet_addr automatically converts "10.12.110.57" to big-endian bytes. 
    // INADDR_ANY

    // inet_addr() bir IP adresini yukarıdaki gibi sayılardan ve noktalardan oluşan bir biçemden alıp unsigned long türünde bir sayıya çevirir. 
    // inet_addr() zaten döndürdüğü değeri Ağ Bayt Sıralamasına göre dizilmiş olarak döndürür yani htonl() işlevini burada çağırmanıza gerek yok.

    memset(&(my_addr.sin_zero), '\0', 8);


    // binding port and address on socket
    errorHandler = bind(sockfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr));

    if(errorHandler == -1) {
        fprintf(stderr, "Error occured on binding port: %d\n", MYPORT);
        return 1;
    }


    // listening socket
    errorHandler = listen(sockfd, BACKLOG);

    if(errorHandler == -1) {
        fprintf(stderr, "Error occured on binding port: %d\n", MYPORT);
        return 1;
    }


    // accepting a request
    sin_size = sizeof(struct sockaddr_in);
    new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size);

    if(new_fd == -1) {
        fprintf(stderr, "Error occured on accepting connection from: %s:%s\n", "", "");
        return 1;
    }


    // sending message through the socket
    char *msg = "Connection was successful.";
    int len, bytes_sent;

    len = strlen(msg);
    bytes_sent = send(new_fd, msg, len, 0);

    if(bytes_sent == -1) {
        fprintf(stderr, "Message couldn't send to receiver.\n");
        return 1;
    }


    // receiving message through the socket
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    while(1) {
        int bytes_recieved = recv(new_fd, buffer, sizeof(buffer), 0);

        if(bytes_recieved == -1 || bytes_recieved == 0) {
            break;
        }
        if(buffer) {
            printf("got data %s", buffer);
            memset(buffer, 0, sizeof(buffer));
        }
    }


    // closing the sockets after the connections are complete and WSACleanup().
    closesocket(sockfd);
    closesocket(new_fd);
    WSACleanup();
    return 0;
}