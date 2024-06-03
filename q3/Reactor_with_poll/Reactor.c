#include <stdlib.h>
#include <string.h>
#include "Reactor.h"
#include <stdio.h>

#define INITIAL_CAPACITY 10

Reactor* createReactor() {
    Reactor *reactor = malloc(sizeof(Reactor));
    reactor->fds = malloc(sizeof(struct pollfd) * INITIAL_CAPACITY);
    reactor->handlers = malloc(sizeof(handler_t) * INITIAL_CAPACITY);
    reactor->size = 0;
    reactor->capacity = INITIAL_CAPACITY;
    reactor->running = 1;
    return reactor;
}

void addFd(Reactor* reactor, int newfd, handler_t handler) {
    if (reactor->size == reactor->capacity) {
        reactor->capacity *= 2;
        reactor->fds = realloc(reactor->fds, sizeof(struct pollfd) * reactor->capacity);
        reactor->handlers = realloc(reactor->handlers, sizeof(handler_t) * reactor->capacity);
    }

    reactor->fds[reactor->size].fd = newfd;
    reactor->fds[reactor->size].events = POLLIN;
    reactor->handlers[reactor->size] = handler;
    reactor->size++;
}

void removeFd(Reactor* reactor, int fd) {
    int i;
    for (i = 0; i < reactor->size; i++) {
        if (reactor->fds[i].fd == fd)
            break;
    }

    if (i != reactor->size) {
        memmove(&reactor->fds[i], &reactor->fds[i + 1], sizeof(struct pollfd) * (reactor->size - i - 1));
        memmove(&reactor->handlers[i], &reactor->handlers[i + 1], sizeof(handler_t) * (reactor->size - i - 1));
        reactor->size--;
    }
}

void *runReactor(void *arg) {
    Reactor *reactor = (Reactor *) arg;

    while (reactor->running) {
        int poll_count = poll(reactor->fds, reactor->size, -1);

        if (poll_count == -1) {
            perror("Error poll():");
            break;
        }

        for (int i = 0; i < reactor->size; i++) {
            if (reactor->fds[i].revents & POLLIN) {
                reactor->handlers[i](reactor, reactor->fds[i].fd);
            }
        }
    }

    return NULL;
}

void startReactor(Reactor *reactor) {
    if (pthread_create(&reactor->thread_id, NULL, runReactor, reactor) != 0) {
        perror("Error creating thread");
        exit(EXIT_FAILURE);
    }
}

void stopReactor(Reactor *reactor) {
        if (reactor != NULL) {
            reactor->running = 0;
            waitFor(reactor);
            free(reactor->fds);
            free(reactor->handlers);
            free(reactor);
        }
    }

void waitFor(Reactor *reactor) {
    pthread_join(reactor->thread_id, NULL);
}