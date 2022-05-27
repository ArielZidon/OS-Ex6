
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
#include <pthread.h> 
#include "Active_Object.h"

 //gcc -pthread  server.c Active_Object.h Active_Object.c main1.c server.h

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold

void* f_func(void* temp);//kasare code


void* s_func(void* temp);// a->A , b->B ...


void* t_func(void* temp); // just a func for the last call to "third"


void* queue_insert(void* temp);

void* sendD(void* temp);