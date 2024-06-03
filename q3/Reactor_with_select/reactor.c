#include <stdlib.h>
#include <string.h>
#include "reactor.h"
#include <stdio.h>
#include <sys/select.h>

#define INITIAL_CAPACITY 10

Reactor* createReactor() {
    Reactor *reactor = malloc(sizeof(Reactor));
    reactor->fds = malloc(sizeof(fd_set) * 3); // Array of three fd_sets
    FD_ZERO(&reactor->fds[0]); // Initialize read set
    FD_ZERO(&reactor->fds[1]); // Initialize write set
    FD_ZERO(&reactor->fds[2]); // Initialize except set
    reactor->handlers = malloc(sizeof(handler_t) * INITIAL_CAPACITY);
    reactor->size = 0;
    reactor->capacity = INITIAL_CAPACITY;
    reactor->running = 1;
    return reactor;
}

void addFd(Reactor* reactor, int newfd, handler_t handler) {
    if (reactor->size == reactor->capacity) {
        reactor->capacity *= 2;
        reactor->fds = realloc(reactor->fds, sizeof(fd_set) * 3 * reactor->capacity);
        reactor->handlers = realloc(reactor->handlers, sizeof(handler_t) * reactor->capacity);
    }

    reactor->handlers[reactor->size] = handler;
    reactor->size++;
    FD_SET(newfd, &reactor->fds[0]); // Add to read set initially
}

void removeFd(Reactor* reactor, int fd) {
    int i;
    for (i = 0; i < reactor->size; i++) {
        if (FD_ISSET(fd, &reactor->fds[0])) // Check read set first
            break;
    }

    if (i == reactor->size) { // Not found in read set, check others
        for (i = 0; i < reactor->size; i++) {
            if (FD_ISSET(fd, &reactor->fds[1]) || FD_ISSET(fd, &reactor->fds[2]))
                break;
        }
    }

    if (i != reactor->size) {
        FD_CLR(fd, &reactor->fds[0]); // Remove from read set
        FD_CLR(fd, &reactor->fds[1]); // Remove from write set (just in case)
        FD_CLR(fd, &reactor->fds[2]); // Remove from except set (just in case)
        memmove(&reactor->fds[0], &reactor->fds[0] + 1, sizeof(fd_set) * 2); // Move remaining sets
        memmove(&reactor->handlers[i], &reactor->handlers[i + 1], sizeof(handler_t) * (reactor->size - i - 1));
        reactor->size--;
    }
}

void *runReactor(void *arg) {
    Reactor *reactor = (Reactor *) arg;
    fd_set active_fds;

    while (reactor->running) {
        // Copy read set for modification within select
        memcpy(&active_fds, &reactor->fds[0], sizeof(fd_set));

        int max_fd = -1;
        for (int i = 0; i < reactor->size; i++) {
            if (FD_ISSET(i, &active_fds) && i > max_fd) {
                max_fd = i;
            }
        }
        if (max_fd == -1) continue; // No file descriptors to process

        int select_count = select(max_fd + 1, &active_fds, NULL, NULL, NULL);  // No timeout

        if (select_count == -1) {
            perror("Error select():");
            break;
        }

        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &active_fds)) {
                reactor->handlers[i](reactor, i);
            }
        }
    }

    return NULL;
}
void startReactor(Reactor *reactor) {
    runReactor(reactor);
}

void stopReactor(Reactor *reactor) {
    if (reactor != NULL) {
        reactor->running = 0;
        free(reactor->handlers);
        free(reactor);
    }
}

// Rest of the code remains the same (startReactor, stopReactor, waitFor)
