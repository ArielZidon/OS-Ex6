#include "Active_Object.h"
#include "Queue.h"

#include <stdlib.h>
#include <stdio.h>


void* runAO(void* temp) {
    AO *active_Obj = (AO*)temp;
    while (active_Obj->run) { 
        void* handled_now = active_Obj->f1(deQ(active_Obj->Q)); // wait on cond
        void* result = active_Obj->f2(handled_now);
    }
    free(active_Obj->p);
    free(active_Obj);
}

AO* newAO(queue* Q, void* f1, void* f2) {
    AO *active_Obj = (AO*)malloc(sizeof(AO));
    active_Obj->f1 = f1;
    active_Obj->f2 = f2;
    active_Obj->Q = Q;
    active_Obj->run = 1;
    active_Obj->p = (pthread_t*)malloc(sizeof(pthread_t));
    pthread_create(active_Obj->p,NULL, runAO, (void*)active_Obj);
    return active_Obj;
}

void destroyAO(AO* active_Obj) {
    active_Obj->run=false;
    free(active_Obj->p);
    free(active_Obj);
}