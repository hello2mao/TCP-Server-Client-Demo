#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <pthread.h>
#include <arpa/inet.h>

#define MAX_READ_BUF_LEN 80
#define MAX_WITE_BUF_LEN 80
#define PORT 8080

void *handler(void *arg)
{
    int sockfd = (int)arg;
    int nbytes;
    char read_buff[MAX_READ_BUF_LEN];
    bzero(read_buff, MAX_READ_BUF_LEN);

    // read the message from client and copy it in buffer
    nbytes = recv(sockfd, read_buff, sizeof(read_buff), 0);
    if (nbytes < 0)
    {
        /* Read error. */
        perror("read err");
        close(sockfd);
        return NULL;
    }
    else if (nbytes == 0)
    {
        /* End-of-file. */
        close(sockfd);
        return NULL;
    }
    else
    {
        /* Data read. */
        printf("From client: %s\n", read_buff);
        close(sockfd);
        return NULL;
    }
}

int main()
{
    int sockfd;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    printf("Socket successfully created..\n");

    // assign IP, PORT
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    printf("Socket successfully binded..\n");

    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    printf("Server listening on port: %d\n", PORT);

    for (;;)
    {

        // Accept the data packet from client and verification
        socklen_t len = sizeof(cli);
        int connection_fd = accept(sockfd, (struct sockaddr *)&cli, &len);
        if (connection_fd < 0)
        {
            printf("server acccept failed...\n");
            exit(0);
        }
        char buf_ip[INET_ADDRSTRLEN];
        memset(buf_ip, '\0', sizeof(buf_ip));
        inet_ntop(AF_INET, &cli.sin_addr, buf_ip, sizeof(buf_ip));
        printf("get connect, ip: %s, port: %d\n", buf_ip, ntohs(cli.sin_port));

        // create a thread to handler this connection
        // TODO: change to epoll or thread pool
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, (void *)handler, (void *)connection_fd);
        pthread_detach(thread_id);
    }

    // close the socket
    close(sockfd);
    return 0;
}
