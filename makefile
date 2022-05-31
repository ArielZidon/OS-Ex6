.PHONY: all run clean
CC = gcc
CXX= g++
FLAGS= 
HEADERS = Queue.h Active_Object.h 
all: server main1 client Active_Object 


client: client.o 
	$(CC) $< -o client
	
server: server.o main1.o Active_Object.o
	$(CC) $< main1.o Active_Object.o -o server -lpthread

Queue: main1.o $(HEADERS)
	$(CC) $< -o main1 -lpthread

Active_Object: Active_Object.o $(HEADERS)
	$(CC) $< -o Active_Object -lpthread main1
	
%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o server client Active_Object