#ifndef SERVER_H
#define	SERVER_H

#include <signal.h>

#include "../message.h"
//Constants
#define BACKLOG 10 // how many pending connections queue will hold

void sigchld_handler(int s);
int start_server(char* port);
void listen_server(int sockfd);

void handle_message_s(struct lab3message* message, int fd);
int check_user(char* id, char* pw);

void login_s(struct lab3message* message, int sender_fd);
void exit_s(struct lab3message* message, int sender_fd);
void join_s(struct lab3message* message, int sender_fd);
void leave_s(struct lab3message* message, int sender_fd);
void create_s(struct lab3message* message, int sender_fd);
void text_s(struct lab3message* message, int sender_fd);
void query_s(struct lab3message* message, int sender_fd);
void invite_s(struct lab3message* message, int sender_fd); 
void join_on_invite(struct lab3message* message, int sender_fd);
void notify_inviter_of_rejection(struct lab3message* message, int sender_fd);

#endif	/* SERVER_H */
