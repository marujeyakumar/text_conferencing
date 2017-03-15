all: client server

client: client.c client.h message.c message.h
	gcc -o client client.c message.c -I.

server: server.c server.h message.c message.h
	gcc -o server server.c message.c  -I.
