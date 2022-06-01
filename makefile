.PHONY: all run clean
CC = gcc
GG = g++
FLAGS= -pthread
HEADERS = 
all: clientsTest Guard singleton SelectClient PollServer server client

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

SelectClient: SelectClient.o
	$(CC) $<  -o SelectClient $(FLAGS)

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o main1 clientsTest Guard singleton SelectClient Active_Object PollServer server client