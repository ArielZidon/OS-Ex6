#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct QNode {
	void* key;
	struct QNode* next;
}QNode;

typedef struct Queue {
	int size;
	struct QNode *front, *rear;
}Queue;

struct QNode* newNode(void* k);

struct Queue* createQ();

void enQ(struct Queue* q, void* k);

void deQ(struct Queue* q);

void destoryQ(struct Queue *q);
