#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

typedef struct QNode {
	void* key;
	struct QNode* next;
}QNode;

typedef struct Queue {
	int size;
	struct QNode *front, *rear;
}Queue;

struct QNode* newNode(void* k)
{
	struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode));
	temp->key = k;
	temp->next = NULL;
	return temp;
}

// A utility function to create an empty queue
struct Queue* createQ()
{ 
	struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
	q->front = q->rear = NULL;
	q->size = 0;
	return q;
}

// The function to add a key k to q
void enQ(struct Queue* q, void* k)
{
	struct QNode* temp = newNode(k);

	if (q->rear == NULL) {
		q->front = q->rear = temp;
        pthread_cond_signal(&cond);
		q->size++;
		return;
	}
	q->rear->next = temp;
	q->rear = temp;
	q->size++;
}

// Function to remove a key from given queue q
void deQ(struct Queue* q)
{
    pthread_mutex_lock(&mut);
    
    if(q->front == NULL)
    {
    pthread_cond_wait(&cond,&mut);
    }

	// Store previous front and move front one node ahead
	struct QNode* temp = q->front;
	q->front = q->front->next;
	// If front becomes NULL, then change rear also as NULL
	if (q->front == NULL)
		    q->rear = NULL;

	free(temp);
    q->size--;
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

// Driver Program to test anove functions
int main()
{
        
    struct Queue *q = createQ();
    int t = 10;
    pthread_t p1,p2;
    void * temp[2] = {q, &t};

    if(pthread_create(&p1,NULL,&enQ,temp) != 0)
    {
        return 1;
    }
        
    if(pthread_create(&p2,NULL,&deQ,q) != 0)
    {
        return 1;
    }
    pthread_join(p1 , NULL);
    
	enQ(q, 10);
	enQ(q, 20);
	deQ(q);
	deQ(q);
	enQ(q, 30);
	enQ(q, 40);
	enQ(q, 50);
	deQ(q);
	printf("Queue Front : %d \n", q->front->key);
	printf("Queue Rear : %d\n", q->rear->key);
    destoryQ(q);
    pthread_mutex_destroy(&mut);
    pthread_cond_destroy(&cond);
	return 0;
}
