#include "main1.c"

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;
Queue *q1;
Queue *q2;
Queue *q3;

void funk1_2(void* temp)
{
    enQ(q2,temp);
    if(q2->size==1)
    {   
        pthread_cond_signal(&cond2);
    }
}

void* funk1_1(void* temp,void* after_funk)
{
    pthread_mutex_lock(&mut);

    void (*after_funk)(void*) = &funk1_2;

    pthread_mutex_unlock(&mut);
}

void newAO_1(Queue q,void* first_funk,void* after_funk)
{   
    q1 = &q;
    void (*first_funk)(void*) = &funk1_1;
    pthread_t p1;

    while(1)
    {
        if(q.size == 0)
        {
            pthread_cond_wait(&cond1,&mut);
        }
        void* temp = q.front;
        deQ(&q);
        if(pthread_create(&p1,NULL,&first_funk,NULL) != 0)
        {
            return 1;
        }
    }
}



/******************************************************
                     *
                     *
                     * 
                     * 
                     * 
                     * 
        *            *           *
          *          *         *
             *       *       *
                *    *     *  
                  *  * */ 



void funk2_2(void* temp)
{
    enQ(q3,temp);
    if(q3->size==1)
    {   
        pthread_cond_signal(&cond3);
    }
}

void* funk2_1(void* temp,void* after_funk)
{
    pthread_mutex_lock(&mut);
    void (*after_funk)(void*) = &funk2_2;
    pthread_mutex_unlock(&mut);
}

void newAO_2(Queue q,void* first_funk,void* after_funk)
{   
    q2= &q;
    void (*first_funk)(void*) = &funk2_1;
    pthread_t p1;

    while(1)
    {
        if(q.size == 0)
        { 
            pthread_cond_wait(&cond2,&mut);
        }
        void* temp = q.front;
        deQ(&q);
        if(pthread_create(&p1,NULL,&first_funk,NULL) != 0)
        {
            return 1;
        }
    }
}


/******************************************************
                     *
                     *
                     * 
                     * 
                     * 
                     * 
        *            *           *
          *          *         *
             *       *       *
                *    *     *  
                  *  * */ 



void funk3_2(void* temp)
{
    //back to the client
}

void* funk3_1(void* temp,void* after_funk)
{
    pthread_mutex_lock(&mut);
    void (*after_funk)(void*) = &funk3_2;
    pthread_mutex_unlock(&mut);
}

void newAO_2(Queue q,void* first_funk,void* after_funk)
{   
    q3 = &q;
    void (*first_funk)(void*) = &funk3_1;
    pthread_t p1;

    while(1)
    {
        if(q.size == 0)
        { 
            pthread_cond_wait(&cond3,&mut);
        }
        void* temp = q.front;
        deQ(&q);
        if(pthread_create(&p1,NULL,&first_funk,NULL) != 0)
        {
            return 1;
        }
    }
}