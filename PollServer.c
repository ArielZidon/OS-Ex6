/*
** pollserver.c -- a cheezy multiperson chat server
** take from https://beej.us/guide/bgnet/html/#poll
*/
#define PORT "3491" 
#include "Reactor.h"

int fd_count,l;
struct pollfd *pfds;
char buf[1024];

// Get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}
preactor newReactor()
{
    preactor res = (preactor)(malloc(sizeof(reactor)));
    return res;
}
void InstallHandler(preactor rc, pfunc newFunc, int file_des)
{
    rc->func = newFunc;
    rc->fileID = file_des;
    preqests rq = (preqests)(malloc(sizeof(reqests)));
    rq->fileID = file_des;
    rq->reac = rc;
    pthread_create(&rc->threadID, NULL, newFunc, rq);
}
void RemoveHandler(preactor reac, int fd_free)
{
    pthread_join(reac->threadID, NULL);
    reac->fileID = -1;
    reac->func = NULL;
}

// Return a listening socket
int get_l_socket(void)
{
    int l; // Listening socket descriptor
    int yes = 1;  // For setsockopt() SO_REUSEADDR, below
    int rv;

    struct addrinfo hints, *ai, *p;

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0)
    {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next)
    {
        l = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (l < 0)
        {
            continue;
        }

        // Lose the pesky "address already in use" error message
        setsockopt(l, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(l, p->ai_addr, p->ai_addrlen) < 0)
        {
            close(l);
            continue;
        }

        break;
    }

    freeaddrinfo(ai); // All done with this

    // If we got here, it means we didn't get bound
    if (p == NULL)
    {
        return -1;
    }

    // Listen
    if (listen(l, 10) == -1)
    {
        return -1;
    }

    return l;
}

// Add a new file descriptor to the set
void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size)
    {
        *fd_size *= 2; // Double it

        *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;
}

// RemoveHandler an index from the set
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count - 1];

    (*fd_count)--;
}
void *myTreadFunc(void *arg)
{
    preqests req = &((preqests)arg)[0];
    int  newFd = req->fileID;
    char buf[1024]; // Buffer for client data
    for(;;)
    {
        int bytes = recv(newFd, buf, sizeof(buf), 0);
        if (bytes <= 0)
        {
            printf("pollserver: socket %d disconnected\n", newFd);
            close(newFd);
            return NULL;
        }
        else
        {
            for (int i = 0; i < fd_count + 1; i++)
            {
                int client_fd = pfds[i].fd;
                // printf("fd_count: %d , client_fd: %d, l: %d ,new_fd: %d ", fd_count, client_fd, l, new_fd);
                if (client_fd != l && client_fd != newFd)
                {
                    send(client_fd, buf, bytes, 0);
                }
            }
            bzero(buf, 1024);
        }
    }
}
// Main
int main(void)
{
    int newfd;                          // Newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen;


    char remoteIP[INET6_ADDRSTRLEN];

    // Start off with room for 5 connections
    // (We'll realloc as necessary)
    fd_count = 0;
    int fd_size = 5;
    pfds = (struct pollfd *)malloc(sizeof(*pfds) * fd_size);

    // Set up and get a listening socket
    l = get_l_socket();

    if (l == -1)
    {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    // Add the l to set
    pfds[0].fd = l;
    pfds[0].events = POLLIN; // Report ready to read on incoming connection

    fd_count = 1; // For the l

    printf("Server is ready:\n");
    // Main loop
    for (;;)
    {
        int poll_count = poll(pfds, fd_count, -1);

        if (poll_count == -1)
        {
            perror("poll");
            exit(1);
        }

        // Run through the existing connections looking for data to read
        for (int i = 0; i < fd_count; i++)
        {

            // Check if someone's ready to read
            if (pfds[i].revents & POLLIN)
            { // We got one!!

                if (pfds[i].fd == l)
                {
                    // If l is ready to read, handle new connection

                    addrlen = sizeof remoteaddr;
                    newfd = accept(l,
                                   (struct sockaddr *)&remoteaddr,
                                   &addrlen);

                    if (newfd == -1)
                    {
                        perror("accept");
                    }
                    else
                    {
                        add_to_pfds(&pfds, newfd, &fd_count, &fd_size);

                        printf("pollserver: new connection from %s on "
                               "socket %d\n",
                               inet_ntop(remoteaddr.ss_family,
                                         get_in_addr((struct sockaddr *)&remoteaddr),
                                         remoteIP, INET6_ADDRSTRLEN),
                               newfd);
                        preactor reac = (preactor)newReactor();
                        InstallHandler(reac, &myTreadFunc, newfd);
                    }
                }
                else
                {
                    // If not the l, we're just a regular client
                    int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);

                    int sender_fd = pfds[i].fd;

                    if (nbytes <= 0)
                    {
                        // Got error or connection closed by client
                        if (nbytes == 0)
                        {
                            // Connection closed
                            printf("pollserver: socket %d hung up\n", sender_fd);
                        }
                        else
                        {
                            perror("recv");
                        }

                        close(pfds[i].fd); // Bye!

                        del_from_pfds(pfds, i, &fd_count);
                    }
                    else
                    {
                        // We got some good data from a client

                        for (int j = 0; j < fd_count; j++)
                        {
                            // Send to everyone!
                            int dest_fd = pfds[j].fd;

                            // Except the l and ourselves
                            if (dest_fd != l && dest_fd != sender_fd)
                            {
                                if (send(dest_fd, buf, nbytes, 0) == -1)
                                {
                                    perror("send");
                                }
                            }
                        }
                    }
                } // END handle data from client
            }     // END got ready-to-read from poll()
        }         // END looping through file descriptors
    }             // END for(;;)--and you thought it would never end!

    return 0;
}