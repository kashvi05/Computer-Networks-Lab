#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 5555
#define MAX_LINE 8192

// Driver code
int main()
{
    int sockfd;
    char buf[MAX_LINE];
    char *message = "GET";
    struct sockaddr_in sin;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&sin, 0, sizeof(sin));

    // Filling server information
    sin.sin_family = AF_INET;
    sin.sin_port = htons(PORT);
    sin.sin_addr.s_addr = INADDR_ANY;

    int n, f;
    socklen_t len = (socklen_t)sizeof(struct sockaddr_in);

    sendto(sockfd, (const char *)message, strlen(message),
           0, (const struct sockaddr *)&sin,
           sizeof(struct sockaddr_in));
    printf("GET sent.\n");
    int totalBytes = 0;
    n = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&sin, &len); // Receive the filename
    FILE *fp = fopen(buf, "wb");
    if (fp == NULL)
    {
        perror("FILE CREATION FAILED!!!!");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        n = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&sin, &len);
        printf("Recieving - %d\n", n);
        if (strcmp(buf, "EOF") == 0)
        {
            printf("n here");
            break;
        }
        fwrite(buf, 1, MAX_LINE - 1, fp);
        printf("Recieved Bytes:%ld\n", strlen(buf));
        memset(buf, 0, MAX_LINE);
        printf("Server : File recieved\n");
    }

    fclose(fp);
    return 0;
}