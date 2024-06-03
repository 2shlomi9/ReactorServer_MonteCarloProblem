
CC = gcc
CFLAGS = -Wall

all: server client

server: server.o
	$(CC) $(CFLAGS) -o server server.o -lm
client: client.o
	$(CC) $(CFLAGS) -o client client.o -lm

clean:
	rm -f server client *.o
	
