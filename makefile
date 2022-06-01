.PHONY: all run clean
CC = gcc
GG = g++
FLAGS= -pthread
HEADERS = 
all: clientsTest Guard singleton Pclient PollServer server client

server: server.o Active_Object.o main1.o  
	$(CC) server.o Active_Object.o main1.o -o server $(FLAGS)

# Active_Object: main1.o Active_Object.o
# 	$(CC) Active_Object.o main1.o -o Active_Object $(FLAGS)

client: client.o
	$(CC) $< -o client

Guard: 
	$(GG) Guard.cpp -o Guard $(FLAGS)

singleton: 
	$(GG) singleton.cpp -o singleton $(FLAGS)

PollServer: PollServer.o
	$(CC) $<  -o PollServer $(FLAGS)

Pclient: Pclient.o
	$(CC) $<  -o Pclient $(FLAGS)

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o main1 clientsTest Guard singleton Pclient Active_Object PollServer server client