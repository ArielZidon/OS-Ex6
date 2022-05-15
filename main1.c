#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t mut;
pthread_cond_t cond;
pthread_cond_t cond2;
pthread_cond_init(&cond,NULL);
pthread_cond_init(&cond2,NULL);

struct QNode {
	void* key;
	struct QNode* next;
};

struct Queue {
	struct QNode *front, *rear;
};

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
    pthread_mutex_lock(&mut);
	struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
	q->front = q->rear = NULL;
    pthread_cond_signal(&cond2);
    
	return q;
}

// The function to add a key k to q
void enQ(struct Queue* q, void* k)
{
    pthread_mutex_lock(&mut);
	struct QNode* temp = newNode(k);

	if (q->rear == NULL) {
		q->front = q->rear = temp;
		return;
	}
	q->rear->next = temp;
	q->rear = temp;
    pthread_mutex_unlock(&mut);
    pthread_cond_signal(&cond);
}

// Function to remove a key from given queue q
void deQ(struct Queue* q)
{
    pthread_mutex_lock(&mut);
    if(q->front == NULL)
    {
    pthread_cond_wait(&cond,&mut);
    }

	// If queue is empty, return NULL.
	if (q->front == NULL)
		return;
	// Store previous front and move front one node ahead
	struct QNode* temp = q->front;
	q->front = q->front->next;
	// If front becomes NULL, then change rear also as NULL
	if (q->front == NULL)
		q->rear = NULL;

	free(temp);
    
    pthread_mutex_unlock(&mut);
}

void destoryQ(struct Queue *q)
{
    pthread_mutex_lock(&mut);

     if(q == NULL)
    {
        pthread_cond_wait(&cond2,&mut);
    }

    while(q->front!=NULL)
    {
        deQ(q);
    }
    free(q);
    pthread_mutex_unlock(&mut);
}

// Driver Program to test anove functions
int main()
{
    pthread_mutex_init(&mut,NULL);

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
    pthread_join(p2 , NULL);
    

	// enQueue(q, 10);
	// enQueue(q, 20);
	// deQueue(q);
	// deQueue(q);
	// enQueue(q, 30);
	// enQueue(q, 40);
	// enQueue(q, 50);
	// deQueue(q);
	// printf("Queue Front : %d \n", q->front->key);
	// printf("Queue Rear : %d", q->rear->key);

    pthread_mutex_destroy(&mut);
    pthread_cond_destroy(&cond);
    pthread_cond_destroy(&cond2);
	return 0;
}
