#ifndef CLIENT_H
#define	CLIENT_H

#include "message.h"
#include <sys/socket.h>

enum COMMANDS{
	LOG_IN 		   = 0, 
	LOG_OUT         = 1, 
	JOIN_SESSION   = 2,
	LEAVE_SESSION  = 3,
	CREATE_SESSION = 4,
	LIST		   = 5,
	QUIT		   = 6
};

struct status_t {

	int sockfd;
	struct addrinfo *p;

	char client_id[MAXBUFLEN];
	char server_ip[MAXBUFLEN];
	char password[MAXBUFLEN];

	int session_id;
	int logged_in;
} status;

void *get_in_addr(struct sockaddr *sa);
int check_command(char* command);


void login_c(char client_id[MAXBUFLEN], char password[MAXBUFLEN], char server_ip[MAXBUFLEN], int port);
void logout(); 
void join_session(); 
void leave_session(); 
void create_session(); 
void list(); 
void quit();

#endif	/* CLIENT_H */
