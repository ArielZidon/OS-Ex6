#pragma once
#include <stdbool.h>
#include <pthread.h>
#include "Queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define nullptr ((void*)0)


struct node {
    void* value;
    struct node *prev;
    struct node *next;
};

typedef struct queue {
    struct node *front;
    struct node *rear;
    int size;
    pthread_mutex_t mut;
    pthread_cond_t deQ_Wait;
    void* lastVal;
}queue;

queue* createQ();
void destroyQ(queue*);
bool enQ(void*, queue*);
void* deQ(queue*);