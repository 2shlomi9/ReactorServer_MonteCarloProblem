CC = gcc
CFLAGS = -Wall -fPIC
LIBS = -pthread -lm

all: server

server: server.o libreactor.so
	$(CC) $(CFLAGS) -o server server.o Reactor.o $(LIBS)

libreactor.so: Reactor.o
	$(CC) -shared -o libreactor.so Reactor.o

Reactor.o:	../q3/Reactor_with_poll/Reactor.c ../q3/Reactor_with_poll/Reactor.h
	$(CC) $(CFLAGS) -c ../q3/Reactor_with_poll/Reactor.c 

server.o: server.c ../q3/Reactor_with_poll/Reactor.h
	$(CC) $(CFLAGS) -c server.c -lm

clean:
	rm -f server *.o libreactor.so
