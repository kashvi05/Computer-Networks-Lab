#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

#define PORT 5555
#define MAX_LINE 8192

// Driver code
int main()
{
    int sockfd;
    char buf[MAX_LINE];
    struct sockaddr_in sin, cin;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("[-] Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&sin, 0, MAX_LINE);
    memset(&cin, 0, MAX_LINE);

    sin.sin_family = AF_INET; // IPv4
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&sin,
             sizeof(sin)) < 0)
    {
        perror("[-] Binding failed");
        exit(EXIT_FAILURE);
    }

    int n;

    socklen_t len = (socklen_t)sizeof(struct sockaddr_in); // len is value/resuslt

    n = recvfrom(sockfd, buf, MAX_LINE,
                 0, (struct sockaddr *)&cin,
                 &len);
    printf("Client : %s\n", buf);
    if (strcmp(buf, "GET") == 0)
    {
        memset(buf, 0, MAX_LINE);
        char *filename = "video.mp4";
        sendto(sockfd, (const char *)filename, strlen(filename), 0, (const struct sockaddr *)&cin, len);

        FILE *fp = fopen(filename, "rb");
        long int totalBytes = 0;
        int bytesRead;
            struct timespec rem;
            struct timespec req;
            req.tv_sec = 1;
            req.tv_nsec = 0L;

        while (!feof(fp))
        {
            if ((bytesRead = fread(buf, 1, MAX_LINE - 1, fp)) > 0)
            {
                printf("Bytes sent = %d\n", bytesRead);
                totalBytes = totalBytes + bytesRead;
                if (sendto(sockfd, buf, sizeof(buf), 0, (const struct sockaddr *)&cin, len) < 0)
                {
                    perror("[-] File not sent");
                }
                nanosleep(&req,&rem);
            }

            memset(buf, 0, MAX_LINE);
        }

        printf("Total bytes = %ld\n", totalBytes);
        // printf("Total bytes sent: %ld", totalBytes);
        if (ferror(fp) != 0)
        {
            perror("[-] fgets Failed");
        }
        if (feof(fp))
        {
            const char *complete = "EOF";
            sendto(sockfd, (const char *)complete, strlen(complete), 0, (const struct sockaddr *)&cin, len);
            fclose(fp);
        }
    }
    return 0;
}