#ifndef SERVER_H
#define SERVER_H

#include "../q3/Reactor_with_poll/Reactor.h"

int isInsideEllipse(double x, double y, double x1, double y1, double x2, double y2, double radius);
double percentageCovered(double x1, double y1, double x2, double y2, double radius);
void handle_connections(Reactor* reactor,int fd);
void handle_client(Reactor *reactor, int newfd);


#endif