.PHONY: all run clean
CC = gcc
GG = g++
OBJECTS = Queue.o 
FLAGS= -pthread
HEADERS = 
all: clientsTest Guard singleton SelectClient PollServer main1 client lib.so

main1: main1.o Active_Object.o Queue.o  
	$(CC) main1.o Active_Object.o Queue.o -o main1 $(FLAGS)

# Active_Object: main1.o Active_Object.o
# 	$(CC) Active_Object.o main1.o -o Active_Object $(FLAGS)

client: client.o
	$(CC) $< -o client

Guard: Guard.o
	$(GG) Guard.cpp -o Guard $(FLAGS)

singleton: singleton.o
	$(GG) singleton.cpp -o singleton $(FLAGS)

PollServer: PollServer.o
	$(CC) $<  -o PollServer $(FLAGS)

SelectClient: SelectClient.o
	$(CC) $<  -o SelectClient $(FLAGS)

lib.so: $(OBJECTS)
	$(CC) --shared -fPIC -pthread $(OBJECTS) -o lib.so

%.o: %.c
	$(CC) -c $< -o $@

clean:
	rm -f *.o Queue clientsTest Guard singleton SelectClient Active_Object PollServer main1 client lib.so