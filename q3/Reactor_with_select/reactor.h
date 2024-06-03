#ifndef REACTOR_H
#define REACTOR_H

#include <pthread.h>
#include <sys/select.h>

typedef void (*handler_t)(void *self, int);

typedef struct Reactor {
    fd_set *fds;  // Array of three fd_sets for read, write, and except
    handler_t *handlers;
    int size;
    int capacity;
    int running;
    pthread_t thread_id;
} Reactor;

Reactor* createReactor();
void startReactor(Reactor* reactor);
void stopReactor(Reactor* reactor);
void addFd(Reactor* reactor, int newfd, handler_t handler);
void removeFd(Reactor* reactor, int fd);
void waitFor(Reactor *reactor);

#endif
