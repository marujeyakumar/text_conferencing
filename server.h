#ifndef SERVER_H
#define	SERVER_H

#include <signal.h>
#include "message.h"
//Constants
#define BACKLOG 10 // how many pending connections queue will hold



void sigchld_handler(int s);
int start_server(char* port);
void listen_server(int sockfd);

void handle_message_s(struct lab3message* message);
int check_user(char* id, char* pw);

void login_s(struct lab3message* message);
void exit_s(struct lab3message* message);
void join_s(struct lab3message* message);
void leave_s(struct lab3message* message);
void create_s(struct lab3message* message);
void text_s(struct lab3message* message);
void query_s(struct lab3message* message);
#endif	/* SERVER_H */
