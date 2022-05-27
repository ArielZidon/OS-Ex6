#include "Active_Object.h"

void* runf(void* newO)
{
    AO *Active_Object = (AO*)newO;
    while (Active_Object->running) { 
        void* temp = Active_Object->Q->front;
        deQ(Active_Object->Q);
        
        void* runingNow = Active_Object->first_func(temp); 
        void* result = Active_Object->after_func(runingNow);
    }
} 


AO* newAO(Queue *q,void* first_funk,void* after_funk)
{   
    
    AO *actO = (AO*)malloc(sizeof(AO));
    actO->first_func = first_funk;
    actO->after_func = after_funk;
    actO->Q = q;
    actO->running = 1;
    actO->t = (pthread_t*)malloc(sizeof(pthread_t));
    pthread_create(actO->t,NULL, runf, (void*)actO);
    printf("AO is create \n");
    return actO;
}