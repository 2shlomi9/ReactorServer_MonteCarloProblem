CC = gcc
CFLAGS = -Wall -fPIC
LIBS = -pthread -lm

all: server client

server: server.o libreactor.so
	$(CC) $(CFLAGS) -o server server.o Reactor.o $(LIBS)

client: client.o 
	$(CC) $(CFLAGS) -o client client.o $(LIBS)

libreactor.so: Reactor.o
	$(CC) -shared -o libreactor.so Reactor.o

Reactor.o:	../q3/Reactor_with_poll/Reactor.c ../q3/Reactor_with_poll/Reactor.h
	$(CC) $(CFLAGS) -c ../q3/Reactor_with_poll/Reactor.c 

server.o: server.c ../q3/Reactor_with_poll/Reactor.h
	$(CC) $(CFLAGS) -c server.c 

client.o: client.c 
	$(CC) $(CFLAGS) -c client.c 

clean:
	rm -f server client *.o libreactor.so
