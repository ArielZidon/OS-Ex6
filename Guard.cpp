
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <pthread.h>

using namespace std;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

class guard{
    public:
    guard(){ pthread_mutex_lock(&mut);}
    ~guard(){pthread_mutex_unlock(&mut);}
};

void* tryTheGuard(void* num)
{
    guard g;
    cout<<"the guard is gurding"<<endl;
    sleep(10);
    cout<<"the guard is in a rest mode"<<endl;
    cout<<num<<endl;
    return (int*)1;
}

int main(){
    pthread_t p1,p2;
    static int num = 0;
    if(pthread_create(&p1,NULL,&tryTheGuard,&num) != 0)
    {
        return 1;
    }
        
    if(pthread_create(&p2,NULL,&tryTheGuard,&num) != 0)
    {
        return 1;
    }

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
}
//clang++-9 -p_thread Guard.cpp
//you can use gaurd to protect strtok..!!!!!!!!!!