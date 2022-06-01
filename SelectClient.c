#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT "3491" 
#define MAXDATASIZE 20 // max number can get at once 

int con ,sockfd;
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void *rec(void *arg)
{
    char buff[1024] = {0};
    int length = 0;
    while ((length = recv(sockfd, buff, 1024, 0)) != -1)
    {
        if (!length)
        {
            con = 0;
            break;
        }
        printf("\nGET> %s\n",buff);
        if (!strcmp(buff, "exit"))
        {   printf("Client number %d out\n",sockfd);
            con = 0;
            break;
        }
        bzero(buff, 1024);
    }
    return NULL;
}
void *sendFunc(void *arg)
{
    char input[1024] = {0};
    while (con != 0)
    {
        
        printf("SEND>");
        gets(input);
        // printf("\n",);
        if (strncmp(input,"exit",4) == 0)
        {
            send(sockfd,"exit",4,0);
            con = 0;
            break;
        }
        if (send(sockfd, input, strlen(input) + 1, 0) == -1)
        {
            perror("send");
        }
        bzero(input, 1024);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
      
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "connecting failed\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);

    printf("connecting to %s\n", s);

    freeaddrinfo(servinfo);

    pthread_t p1,p2;
    con = 1;
    pthread_create(&p1, NULL, rec, NULL);
    pthread_create(&p2, NULL, sendFunc, NULL);
    pthread_join(p1, NULL);
    pthread_join(p1, NULL);
    pthread_kill(p2, 0);
    pthread_kill(p2, 0);
    close(sockfd);
    return 0;
}