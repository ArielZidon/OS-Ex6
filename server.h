typedef struct unic_fd {
    int stage;
    char txt[1024];
    int fd;
}unic_fd;

void* nopPoent(void* temp);
void* insertQ1(void* temp);
void* insertQ2(void* temp);
void* insertQ3(void* temp);

void* f_func(void* temp);
void* s_func(void* temp);
void* answer(void* temp); 

#define nullptr ((void*)0)
#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold