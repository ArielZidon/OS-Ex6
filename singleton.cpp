#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <pthread.h>

using namespace std;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

template <typename T>
class Singleton
{
    private:
    static Singleton* instance;
    Singleton(T temp);
    T secret;

    public:
    static Singleton *Instance(T temp);
    void Destroy();
};

//*********************************************//
template <typename T>
Singleton<T>::Singleton(T temp)
{
    secret = temp;
}

template <typename T>
Singleton<T> *Singleton<T>::instance = NULL;

template <typename T>
Singleton<T> *Singleton<T>::Instance(T temp)
{
    if (instance == NULL)
    {
        pthread_mutex_lock(&mut);
        if (instance == NULL)
        {
            instance = new Singleton(temp);
        }
        pthread_mutex_unlock(&mut);
    }
    return instance;
}

template <typename T>
void Singleton<T>::Destroy()
{
    delete(instance);
    instance = nullptr;
}




int main()
{
    FILE *temp;
    Singleton<FILE *> *a = Singleton<FILE *>::Instance(temp);
    Singleton<FILE *> *b = Singleton<FILE *>::Instance(temp);
    cout << "first file: "<<a<< endl;
    cout << "second file: "<<b<<"\n"<<endl;
    if (a == b)
    {
        cout << "its work!!, A build the first object!\nB try to build another one but the object was catch by A..\n" << endl;
    }
    else
    {
        cout << "The singelton dosen't works :(" << endl;
    }
}