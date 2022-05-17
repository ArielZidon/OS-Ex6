#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_Active_Object = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_t p3;

void *f1;
void *f2;

struct QNode {
	void* key;
	struct QNode* next;
};

typedef struct Queue {
	struct QNode *front, *rear;
}Queue;

struct QNode* newNode(void* k)
{
	struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode));
	temp->key = k;
	temp->next = NULL;
	return temp;
}

struct Queue* createQ()
{ 
	struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
	q->front = q->rear = NULL;
	return q;
}

void enQ(struct Queue* q, void* k)
{
	struct QNode* temp = newNode(k);
	if (q->rear == NULL) {
		q->front = q->rear = temp;
        // pthread_cond_signal(&cond);
		return;
	}
	q->rear->next = temp;
	q->rear = temp;
}

void deQ(struct Queue* q)
{
    pthread_mutex_lock(&mut);
    if(q->front == NULL)
    {
    pthread_cond_wait(&cond,&mut);
    }
	struct QNode* temp = q->front;
	q->front = q->front->next;
	if (q->front == NULL)
		    q->rear = NULL;
	free(temp);
    pthread_mutex_unlock(&mut);
}


void destoryQ(struct Queue *q)
{
    while(q->front!=NULL)
    {
        deQ(q);
    }
    free(q);
}


/////////////////////////////////////////////////////////////////////////////

void* funk1(struct Queue q)
{
    return q.rear->key;
}

void funk2(int res)
{
    printf("new number in the Queue: %d\n",res);
}

void newAo(Queue *q,void *fa1,void *fa2)
{
    // while(q->front == NULL)
    // {
        // pthread_cond_wait(&cond_Active_Object,&mut);
    // }

    printf("hiiiiiiiiiiiiiiiii ");
    // int res = funk1(*q);
    // funk2(res);
    printf  ("there ");
    return;    
}

///////////////////////////////////////////////////////////////////////////////////////////////////////



int main()
{        
    struct Queue *q = createQ();
    int t = 10;
    pthread_t p1,p2;
    void * temp[2] = {q, &t};
    

     if(pthread_create(&p3,NULL,&newAo,(*q,f1,f2)) != 0)
    {
        return 1;
    }

    if(pthread_create(&p1,NULL,&enQ,temp) != 0)
    {
        return 1;
    }
        
    if(pthread_create(&p2,NULL,&deQ,q) != 0)
    {
        return 1;
    }

    
    pthread_join(p1, NULL);
    // pthread_join(p3, NULL);
   
	enQ(q, 10);
	enQ(q, 20);
	enQ(q, 30);
	enQ(q, 40);
	enQ(q, 50);
	printf("Queue Front : %d \n", q->front->key);
	printf("Queue Rear : %d\n", q->rear->key);
    destoryQ(q);
    pthread_mutex_destroy(&mut);
    pthread_cond_destroy(&cond);
	return 0;
}





