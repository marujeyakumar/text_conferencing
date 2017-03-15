all: client server

client: client.c client.h message.c message.h server_structs.h server_structs.c
	gcc -o client client.c message.c server_structs.c -I.

server: server.c server.h message.c message.h server_structs.h server_structs.c
	gcc -o server server.c message.c server_structs.c -I.
