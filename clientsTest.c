/*
** client.c -- a stream socket client demo
*/
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

#define PORT "3490" // the port client will be connecting to 
#define MAXDATASIZE 2048 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    char *names[7] = {"Ariel","Zidon", "Afik","adiyarden","danielzAKen","dviR","dvirGev"};
    int sockfd, length;
    char buf[1024];
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

    // loop through all the results and connect to the q1 we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
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
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s ,sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure
   
    char str[2048] = {'\0'};
    char temp[2048] = {'\0'};
    bzero(str, 2048);
    for(int i=0;i<20;i++)
    {   
        sleep(1);
        strcpy(str, names[rand() % 7]);
        for (int i = 0; i < strlen(str); i++)
        {
            printf("%c", str[i]);
        }
        printf(" -> ");
        if(send(sockfd, str, strlen(str), 0) == -1){
            printf("ERROR! from client");
            exit(1);
        } 
        if ((length = recv(sockfd, str,2048, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        str[length] = '\0';
        for (int i = 0; i < strlen(str); i++)
        {
            printf("%c", str[i]);
        }
        printf("\n");
        
        //printf("client: received '%s '\n",str);
        bzero(str, 2048);
    }
    
    close(sockfd);
    return 0;
}