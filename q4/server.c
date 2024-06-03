#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <math.h>
#include "../q3/Reactor_with_poll/Reactor.h"

#define M_PI 3.14159265358979323846
#define BACKLOG 10
#define PORT "9034"
#define FILENAME "output.txt"

// Structure to represent an ellipse
typedef struct {
    double x1, y1, x2, y2, radius;
} Ellipse;

// Global variables for tracking message count and covered area
int message_count = 0;
double total_covered_area = 0.0;

// Create Reactor
Reactor *reactor;

// Declaration of the handles functions 
void handle_new_connection(Reactor *reactor, int fd);
void handle_client(Reactor *reactor, int newfd);

Ellipse ellipses[BACKLOG]; // Array to store ellipses from clients

int isInsideEllipse(double x, double y, Ellipse ellipse) {
    double semi_major_axis = ellipse.radius / 2;
    double focis_distance = sqrt(pow(ellipse.x1 - ellipse.x2, 2) + pow(ellipse.y1 - ellipse.y2, 2));
    double semi_minor_axis = sqrt(pow(semi_major_axis, 2) - pow(focis_distance / 2, 2));
    double centerX = (ellipse.x1 + ellipse.x2) / 2;
    double centerY = (ellipse.y1 + ellipse.y2) / 2;
    double normalizedX = x - centerX;
    double normalizedY = y - centerY;
    double ans = (pow(normalizedX, 2) / pow(semi_major_axis, 2)) + (pow(normalizedY, 2) / pow(semi_minor_axis, 2));
    return ans <= 1;
}

double percentageCovered(Ellipse ellipse) {
    int canvasSize = 100;
    int canvasMinX = -50;
    int canvasMinY = -50;
    int totalPoints = 1000;
    int points_inside_ellipse = 0;

    for (int i = 0; i < totalPoints; i++) {
        double x = ((double) rand() / RAND_MAX) * canvasSize + canvasMinX;
        double y = ((double) rand() / RAND_MAX) * canvasSize + canvasMinY;

        if (isInsideEllipse(x, y, ellipse)) {
            points_inside_ellipse++;
        }
    }

    double canvas_area = canvasSize * canvasSize;
    double estimated_ellipse_area = (double) points_inside_ellipse / totalPoints * canvas_area;
    return (estimated_ellipse_area / canvas_area) * 100;
}

double totalCoveredArea(Ellipse ellipses[], int count) {
    int canvasSize = 100;
    int canvasMinX = -50;
    int canvasMinY = -50;
    int totalPoints = 10000;
    int points_inside_any_ellipse = 0;

    for (int i = 0; i < totalPoints; i++) {
        double x = ((double) rand() / RAND_MAX) * canvasSize + canvasMinX;
        double y = ((double) rand() / RAND_MAX) * canvasSize + canvasMinY;

        for (int j = 0; j < count; j++) {
            if (isInsideEllipse(x, y, ellipses[j])) {
                points_inside_any_ellipse++;
                break; // Point is counted only once even if it's inside multiple ellipses
            }
        }
    }

    double canvas_area = canvasSize * canvasSize;
    double estimated_covered_area = (double) points_inside_any_ellipse / totalPoints * canvas_area;
    return (estimated_covered_area / canvas_area) * 100;
}

void handleCtrlZ(int signal) {
    stopReactor(reactor);
    exit(0);
}

void handleCtrlC(int signal) {
    stopReactor(reactor);
    exit(0);
}

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *) sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *) sa)->sin6_addr);
}

void handle_new_connection(Reactor *reactor, int fd) {
    int newfd;
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen;
    char remoteIP[INET6_ADDRSTRLEN];

    addrlen = sizeof(remoteaddr);
    newfd = accept(fd, (struct sockaddr *) &remoteaddr, &addrlen);

    if (newfd == -1) {
        perror("accept");
    } else {
        printf("server: new connection from %s on socket %d\n",
               inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *) &remoteaddr), remoteIP, INET6_ADDRSTRLEN),
               newfd);
        addFd(reactor, newfd, handle_client);
    }
}

void handle_client(Reactor *reactor, int newfd) {
    double x1, y1, x2, y2, radius;
    double data[5];
    int nbytes;

    nbytes = recv(newfd, data, sizeof(data), 0);
    if (nbytes <= 0) {
        if (nbytes == 0) {
            printf("server: socket %d hung up\n", newfd);
        } else {
            perror("recv");
        }
        close(newfd);
        removeFd(reactor, newfd);
    } else {
        message_count++;
        x1 = data[0];
        y1 = data[1];
        x2 = data[2];
        y2 = data[3];
        radius = data[4];

        Ellipse ellipse;
        ellipse.x1 = x1;
        ellipse.y1 = y1;
        ellipse.x2 = x2;
        ellipse.y2 = y2;
        ellipse.radius = radius;

        ellipses[message_count - 1] = ellipse;

        double ans = percentageCovered(ellipse);
        printf("server: receive data :\nx1 = %lf, y1 = %lf, x2 = %lf, y2 = %lf, radius = %lf\n", x1, y1, x2, y2, radius);
        printf("server: percent covered: %0.2f\n", ans);

        if (send(newfd, &ans, sizeof(double), 0) == -1) {
            perror("send");
        }
        printf("server: sent answer to the client\n");

        total_covered_area = totalCoveredArea(ellipses, message_count);

        FILE *file = fopen(FILENAME, "w");
        if (file != NULL) {
            fprintf(file, "Total messages received: %d\n", message_count);
            fprintf(file, "Total covered area: %.2f\n", total_covered_area);
            fclose(file);
        } else {
            perror("File open error");
        }
    }
}

int get_listener_socket(void) {
    int listener;
    int yes = 1;
    int rv;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            continue;
        }

        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    freeaddrinfo(ai);

    if (p == NULL) {
        return -1;
    }

    if (listen(listener, 10) == -1) {
        return -1;
    }
    printf("Server listen to new connections\n");

    return listener;
}

int main(void) {
    int listener;

    listener = get_listener_socket();
    if (listener == -1) {
        perror("Error in get_listener_socket()");
        exit(EXIT_FAILURE);
    }

    reactor = createReactor();
    addFd(reactor, listener, handle_new_connection);

    signal(SIGTSTP, handleCtrlZ);
    signal(SIGINT, handleCtrlC);

    startReactor(reactor);

    while(reactor->running) {
        sleep(1);
    }

    return 0;
}
