
#include "Queue.h" 
#include <pthread.h>

typedef struct active_object {
    void* (*f1)(void*);
    void* (*f2)(void*);
    queue* Q;
    pthread_t *p;
    int run;

}AO;

AO *newAO(queue*, void*, void*);
void* runAO(void* temp);
void destroyAO(AO*);