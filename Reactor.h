#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#define nullptr ((void*)0)
typedef void *(*pfunc)(void *arg);

typedef struct Reactor
{
    int fileID;
    pthread_t threadID;
    pfunc func;
} reactor, *preactor;

typedef struct reqests
{
    int fileID;
    preactor reac;
} reqests, *preqests;

preactor newR();
void Install(preactor rc, pfunc newFunc, int file_des);
void Remove(preactor reac, int fd_free);