#include"Queue.h"

queue* createQ() 
{
    queue* Q = (queue*)malloc(sizeof(queue));
    Q->front = Q->rear = nullptr;
    Q->lastVal = nullptr;
    Q->size = 0;
    pthread_mutex_init(&Q->mut, NULL);
    pthread_cond_init(&Q->deQ_Wait, NULL);
    return Q;
}

void destroyQ(queue* Q) 
{
    while (Q->size!=0) { 
        deQ(Q);
    }
    if (Q->lastVal!=NULL) {
        free(Q->lastVal);
    }
    pthread_cond_broadcast(&Q->deQ_Wait);
    pthread_cond_destroy(&Q->deQ_Wait);
    pthread_mutex_destroy(&Q->mut);
    free(Q);
}

bool enQ(void* n, queue* Q) 
{
    pthread_mutex_lock(&(Q->mut));
    if (Q->size==0) 
    {
        struct node* newNode = (struct node*)malloc(sizeof(struct node));
        newNode->value = malloc(2048);
        memcpy(newNode->value, n, 2048);
        newNode->next = newNode->prev = nullptr;
        Q->front = Q->rear = newNode;
        Q->size++;
        pthread_mutex_unlock(&(Q->mut));
        pthread_cond_signal(&Q->deQ_Wait);
        return true;
    }
    
    struct node* newNode = (struct node*)malloc(sizeof(struct node));
    newNode->value = malloc(2048);
    memcpy(newNode->value, n, 2048);
    Q->rear->prev = newNode;
    newNode->next = Q->rear;
    newNode->prev = nullptr;
    Q->rear = newNode;
    Q->size++;
    pthread_mutex_unlock(&(Q->mut));
    return true; 
}

void* deQ(queue* Q) {
    pthread_mutex_lock(&(Q->mut));
	if (Q->size==-1) {
        return NULL;
    }
    if (Q->size==0) {
        pthread_cond_wait(&Q->deQ_Wait, &Q->mut);
    }
    if (Q->size==0) {
        pthread_exit(NULL);
    }
    if (Q->lastVal!=NULL) {
        free(Q->lastVal);
    }
    if (Q->size==1) {
        Q->lastVal = malloc(2048);
        memcpy(Q->lastVal, Q->front->value, 2048);
        free(Q->front->value);
        free(Q->front);
        Q->size--;
        pthread_mutex_unlock(&(Q->mut));
        return Q->lastVal;
    }
    Q->lastVal = malloc(2048);
    memcpy(Q->lastVal, Q->front->value, 2048);
    struct node* tempNode = Q->front->prev;
    tempNode->next = nullptr;
    free(Q->front->value);
    free(Q->front);
    Q->front = tempNode;
    Q->size--;
    pthread_mutex_unlock(&(Q->mut));
    return Q->lastVal;
}