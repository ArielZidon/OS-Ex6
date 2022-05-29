.PHONY: all run clean
CC = gcc
CXX=clang++-9 -std=c++2a
FLAGS= 
HEADERS = Queue.h Active_Object.h 
all: server main1 client Active_Object


server: server.o $(HEADERS)
	$(CC) $< main1.o Active_Object -o server -lpthread
	
client: client.o
	$(CC) $< -o client

Active_Object: Active_Object.o $(HEADERS)
	$(CC) $< -o Active_Object -lpthread main1

Queue: main1.o $(HEADERS)
	$(CC) $< -o main1 -lpthread
	
%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o server client Active_Object