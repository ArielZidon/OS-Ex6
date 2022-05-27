/*
** server.c -- a stream socket server demo
*/
#include"server.h"

pthread_mutex_t mut_t = PTHREAD_MUTEX_INITIALIZER;

Queue *q1;
Queue *q2;
Queue *q3;
AO *first;
AO *second; 
AO *third;
static int i = 0;

void* clime_a_client(int new_fd)
{
    char buf[1024];
    struct unic_fd *res = (struct unic_fd*)malloc(sizeof(unic_fd));
    while (1)
    {
        int length = recv(new_fd,buf,sizeof(buf),0);
        if(length<=0)
        {
            perror("recv error");
            pthread_exit(NULL);
            break;
        }
        buf[length+1] = '\0';
        res->fd = new_fd;
        memcpy(res->txt,buf, sizeof(buf));
        printf("Got thr text:%s\n from client:%d",res->txt,res->fd);
        queue_insert(res);
    }
    free(res);
    close(new_fd);
}

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

//Active Object func
/*****************************************************************************/


void* f_func(void* temp) //kasare code
{
    unic_fd *t = (unic_fd*)temp;
    while(t->txt[i])
    {
        if(t->txt[i]>='a' && t->txt[i]<='z')
        {
            t->txt[i] = (t->txt[i]-96)%26+97;
        }
        else if (t->txt[i]>='A' && t->txt[i]<='B')
        {
            t->txt[i] = (t->txt[i]-64)%26+65;
        }
        else
        {
            printf("you send me something is not a letter..");
        }
        
    }
    return t;
}

void* s_func(void* temp) // a->A , b->B ...
{
   unic_fd *t = (unic_fd*)temp;

    while(t->txt[i])
    {
        if(t->txt[i]>='a' && t->txt[i]<='z')
        {
            t->txt[i]-=32;
        }
        else if (t->txt[i]>='A' && t->txt[i]<='B')
        {
            t->txt[i]+=32;
        }
        else
        {
            printf("you send me something is not a letter..");
        }
        
    }
    return t;
}

void* t_func(void* temp) // just a func for the last call to "third"
{
    return temp;
}

void* queue_insert(void* temp)
{
    if(i==0)
    {
        i = 1;
        enQ(q2,temp);
    }
    else
    {
        i = 0;
        enQ(q3,temp);
    }
    return temp;
} 

void* sendD(void* temp)
{
    unic_fd *messege = (unic_fd*)temp;
    int new_fd = messege->fd;
    if (send(new_fd, messege->txt, 1024, 0) == -1)  {
        perror("send");
    }
    printf("The text: %s\n sent to client number : %d\n", messege->txt,new_fd);
    return NULL;
}

int main(void)
{
    q1 = createQ(); 
    q2 = createQ();
    q3 = createQ();
    first = newAO(q1,f_func,queue_insert);
    second = newAO(q1,s_func,queue_insert);
    third = newAO(q1,t_func,sendD);

    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
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

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
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

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
        
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
        pthread_t t;
        if(pthread_create(&t,NULL,&clime_a_client,new_fd) !=0)
        {
            return 1;
        }
    }

    return 0;
}