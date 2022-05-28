# Makefile for socket programs

gcc all: server client

server.o:	server.c
	gcc -o server.o -c server.c
client.o: 	client.c
	gcc -o client.o -c client.c
	

#clean function
clean:
	/bin/rm -rf client server
