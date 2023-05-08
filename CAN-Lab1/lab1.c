#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define PORT 80
#define MAXLINE 1000000

int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serverAddress;
    char input[MAXLINE];
    printf("Enter the hostname: ");
    scanf("%s", input);

    const char *divider = "/";
    char *host_name = strtok(input, divider);
    char *url = strtok(NULL, divider);
    
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    struct hostent *h = gethostbyname(host_name);
    if(h == NULL) {
        printf("failed\n");
        return -1;
    }
    memcpy(&serverAddress.sin_addr, h->h_addr_list[0], h->h_length);

    if (connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    char buffer[MAXLINE] = {0};
    char *msg = malloc(sizeof(char) * 1024);

    strcpy(msg, "GET");
    if(url != NULL) {
        strcat(msg, " /");
        strcat(msg, url);
    }
    
    strcat(msg, " HTTP/1.1\r\nHost: ");
    strcat(msg, host_name);
    strcat(msg, "\r\n\r\n");
    
    printf("socket: Start send HTTP request\n");
    send(sock , msg, strlen(msg), 0);

    printf("socket: Start read the response\n");
    valread = recv(sock , buffer, MAXLINE, MSG_WAITALL);
    printf("socket: Finish read to buffer\n");

    // printf("%s", buffer);
    
    const char *header_start = "<";
    int hyper_count = 0;

    printf("======== Hyperlinks ========\n");

    char *hyper_buffer = strtok(buffer, header_start);
    while (hyper_buffer != NULL) {
        if(hyper_buffer[0] == 'a' && hyper_buffer[1] == ' ' &&  hyper_buffer[2] == 'h' && hyper_buffer[3] == 'r' && hyper_buffer[4] == 'e' && hyper_buffer[5] == 'f') { 
            hyper_count++;
            int i = 8;
            while(hyper_buffer[i] != '\"') {
                printf("%c", hyper_buffer[i]);
                i++;
            }
            printf("\n");
        }
        hyper_buffer = strtok(NULL, header_start);
    }

    printf("\nThere are %d hyperlinks in http://%s\n", hyper_count, input);
    

    return 0;
}