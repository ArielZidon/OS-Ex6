#include "Queue.h" 
#include <pthread.h>

typedef struct AO {

    void* (*first_func)(void*);
    void* (*after_func)(void*);
    Queue* Q;
    pthread_t *t;
    int running;

}AO;

typedef struct unic_fd
{
    int fd;
    char txt [1024];
}unic_fd;

AO *newAO(Queue*, void*, void*);

void* runf(void*);

void destroyAO(AO*);