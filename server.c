#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h> // threads

#include "Queue.h"
#include "Active_Object.h"
#include "server.h"


pthread_t thread_id[BACKLOG];
int new_fd[BACKLOG];

int sockfd; 
bool on;
queue* q1;
queue* q2;
queue* q3;
AO *first;
AO *second; 
AO *third;


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void *clientThread(void *newfd) {
    int new_fd = *(int*)newfd;
    pthread_detach(pthread_self());
    char buf[1024];
    bool co = true;
    unic_fd *res = nullptr;

    while (co&&on) 
    {
        int length = recv(new_fd, buf, sizeof(buf), 0);
        if (length <=0) {
            perror("recv");
            co = 0;
            pthread_exit(NULL);
        }
        buf[length] = '\0';
        res = (unic_fd*)malloc(sizeof(unic_fd));
        res->fd = new_fd;
        memcpy(res->txt, buf, sizeof(buf));
        res ->stage = 0;
        printf("New text %s\nfrom client number: %d\n", res->txt, res->fd);
        insertQ1(res);
    }
    free(res);
    close(new_fd);
}

void sig_handler(int signum)
{
    if (signum == SIGINT) {
        printf("cancel all\n");
        on = 0;
        for (size_t i = 0; i < BACKLOG; i++)
        {
            close(new_fd[i]);
        }

        close(sockfd);
        for (size_t i = 0; i < BACKLOG; i++)
        {
            pthread_cancel(thread_id[i]);
        }
        destroyAO(first);
        destroyAO(second);
        destroyAO(third);
        destroyQ(q1);
        destroyQ(q2);
        destroyQ(q3);
        exit(1);
    }
}

//Active Object func
/*****************************************************************************/

void* insertQ1(void* temp) {
    enQ(temp, q1);
    return temp;
}

void* insertQ2(void* temp) {
    enQ(temp, q2);
    return temp;
}

void* insertQ3(void* temp) {
    enQ(temp, q3);
    return temp;
}

void* nopPoent(void* temp) {
    return temp;
}

void* f_func(void* temp) 
{
    unic_fd* res = (unic_fd* )temp;
    int size = strlen(res->txt);

    for (size_t j = 0; j < size; j++)
    {
        if (res->txt[j]>='a'&&res->txt[j]<='z') {
            res->txt[j] = (res->txt[j]-96)%26+97;
        } 
        else if (res->txt[j]>='A'&&res->txt[j]<='Z') {
            res->txt[j] = (res->txt[j]-64)%26+65;
        }
    }
    return res;
}

void* s_func(void* temp) 
{
    unic_fd *res = (unic_fd*)temp;
    int size = strlen(res->txt);

    for (size_t j = 0; j < size; j++)
    {
        if (res->txt[j]>='a'&&res->txt[j]<='z') {
            res->txt[j] -= 32;
        } 
        else if (res->txt[j]>='A'&&res->txt[j]<='Z') {
            res->txt[j] += 32;
        }
    }
    return temp;
}


void* answer(void* temp) 
{
    unic_fd* res = (unic_fd*)temp;
    int new_fd = res->fd;
    if (send(new_fd, res->txt, 2048, 0) == -1)  
    {
        perror("error by send");
    }
    printf("%s has sent to client number: %d\n", res->txt,new_fd);
    return temp;
}

//////////////////////////////////////////////////////////////////////////////////
int main(void)
{
    q1 = createQ();
    q2 = createQ();
    q3 = createQ();
    first = newAO(q1, f_func,insertQ2);
    second = newAO(q2, s_func , insertQ3);
    third = newAO(q3, nopPoent, answer);
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;

    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the q1 we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections...\n");
    int j = 0;
    signal (SIGINT,sig_handler);
    on = true;

        while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd[j] = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd[j] == -1) {
            perror("accept");
            continue;
        }
        // need to fix connection to a new client
        inet_ntop(their_addr.ss_family,
        get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);

        pthread_create(&thread_id[j%BACKLOG], NULL, clientThread, &new_fd[j]);
        j++;
    }

    return 0;
}