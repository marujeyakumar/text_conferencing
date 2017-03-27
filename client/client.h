#ifndef CLIENT_H
#define CLIENT_H

#include <sys/socket.h>

#include "../message.h"

/*enum COMMANDS {
    LOG_IN = 0,
    LOG_OUT = 1,
    JOIN_SESSION = 2,
    LEAVE_SESSION = 3,
    CREATE_SESSION = 4,
    LIST = 5,
    QUIT = 6
 MESSAGE = 7
};*/

struct status_t {
    int sockfd;
    struct addrinfo *p;

    char client_id[MAXBUFLEN];
    char server_ip[MAXBUFLEN];
    char password[MAXBUFLEN];

    char session_id[MAXBUFLEN];
    int logged_in;
} status;


void *get_in_addr(struct sockaddr *sa);
int handle_commands_c();
int check_command(char* command);


void login_c(char client_id[MAXBUFLEN], char password[MAXBUFLEN], char server_ip[MAXBUFLEN], int port);
void logout();
void join_session(char session_id[MAXBUFLEN]);
void leave_session();
void create_session(char session_id[MAXBUFLEN]);
void list();
void quit();
void text_c(char text[MAXBUFLEN]);
void send_invite(char invitee[MAXBUFLEN]); 
void respond_to_invite(char session[MAXBUFLEN]);
#endif /* CLIENT_H */
