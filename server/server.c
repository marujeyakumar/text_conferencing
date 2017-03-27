#define SERVER_PRINT

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <wait.h>

#include "server.h"
#include "server_structs.h"
#include "../message.h"

int newfd;
struct addrinfo *p;
struct sockaddr_storage their_addr; // connector's address information
fd_set master;


int main(int argc, char *argv[]) {
    
    if (argc < 2) {
        printf("Server: usage: server server_portnumber\n");
        exit(1);
    }
    
    //active_sessions = (Session_t*) malloc(sizeof(Session_t));
    //active_users = (User_t*) malloc(sizeof(User_t));
    //active_sessions;
    //active_users;
    printf("Server: Server Started\n");
    int sockfd = start_server(argv[1]);
    listen_server(sockfd);
    
    free(active_sessions);
    free(active_users);
}

void sigchld_handler(int s) {
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

int start_server(char* port) {
    int sockfd;
    struct addrinfo hints, *servinfo;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;


    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;


    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (int)) == 1) {
            perror("setsockopt: ");
            exit(1);
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    // reap all dead processes
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    return sockfd;
}

void listen_server(int sockfd) {
    int numbytes, i, client_socket[5], max_clients = 5;
    struct lab3message new_mess;

    fd_set readfds;

    FD_ZERO(&master);
    FD_ZERO(&readfds);

    FD_SET(sockfd, &master);


    while (1) {

        readfds = master; //update readfds 
        //block until input arrives on one or more active sockets
        if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) < 0) {
            perror("select: ");
            exit(1);
        }
        //Service all sockets with input pending
        for (i = 0; i < FD_SETSIZE; ++i) {
            newfd = i;

            if (FD_ISSET(i, &readfds)) { //ready to be read

                if (i == sockfd) { //new connection
                    socklen_t addrlen = sizeof (their_addr);
                    if ((newfd = accept(sockfd, (struct sockaddr *) &their_addr, (socklen_t*) & addrlen)) == -1) {
                        perror("accept: ");
                        exit(1);
                    }
                    printf("Server: New connection at sockfd %d\n", newfd);
                    FD_SET(newfd, &master); // add to master set
                }
                
                receive_message(&new_mess, newfd);
                printf("Server: %o %o %s %s\n", new_mess.type, new_mess.size, new_mess.source, new_mess.data);
                handle_message_s(&new_mess, newfd);
            }
        }
    }
}

void handle_message_s(struct lab3message* message, int fd) {
    
    switch (message -> type) {
        case(LOGIN):
            login_s(message, fd);
            break;
        case(EXIT):
            exit_s(message, fd);
            break;
        case(JOIN):
            join_s(message, fd);
            break;
        case(LEAVE_SESS):
            leave_s(message, fd);
            break;
        case(NEW_SESS):
            create_s(message, fd);
            break;
        case(MESSAGE): 
            text_s(message, fd);
            break;
        case(QUERY): 
            query_s(message, fd);
            break;
        case(INVITE): 
            invite_s(message, fd);
            break;        
        default: printf("Server: Client sent invalid packet type\n");
    }
}

int check_user(char* id, char* pw){
    if( strcmp(id, "jean")==0 ) {
        if(strcmp(pw, "pw")==0){
            if(find_user(id, active_users)==NULL){ return 0; }
            else { return 3; } 
        } else return 1; //bad pw
    } else if ( strcmp(id, "maru")==0 ) {
        if(strcmp(pw, "pw")==0){
            if(find_user(id, active_users)==NULL){ return 0; }
            else { return 3; } 
        } else return 1; //bad pw
    }
    return 2; //no user
}


void login_s(struct lab3message* message, int sender_fd) {
    char *token;
    User_t* new_user = (User_t*) malloc(sizeof(User_t));

    token = strtok(message->data, " ");
    strcpy(new_user->client_id, token);
    token = strtok(NULL, "");
    strcpy(new_user->password, token);
    char sess[MAXBUFLEN];
    strcpy(sess, "");
    strcpy(new_user->cur_session, sess);
    new_user->next = NULL;
	new_user->sockfd = sender_fd;

    int valid_user_pw = check_user(new_user->client_id, new_user->password);

    struct lab3message packet;
    char data [MAXBUFLEN];
    packet.type = LO_NACK;

    if (valid_user_pw == 0) {
        packet.type = LO_ACK;
        char list[MAXBUFLEN];
        print_users(list, active_users);
        active_users = add_user(new_user, active_users);
        printf("Server: (in login) active users %s\n", active_users->client_id);

    } else if (valid_user_pw == 1) {
        sprintf(data, "incorrect password");
    } else if (valid_user_pw == 2) {
        sprintf(data, "unauthorized user");
    } else if (valid_user_pw == 3) {
        sprintf(data, "user already logged in");
    }

    packet.size = sizeof (packet.data);
    strcpy(packet.data, data);

    deliver_message(&packet, newfd);
}

void exit_s(struct lab3message* message, int sender_fd) {
    active_users = delete_user(message->source, active_users);
    FD_CLR(sender_fd, &master); 
}

void join_s(struct lab3message* message, int sender_fd) {
    printf("----------JOIN SESSION ------");
    struct session_t* session_to_join = find_session(message->data, active_sessions);
            //printf("Server: (join) active users %s with session %s\n", active_users->client_id, active_users->cur_session);
            //printf("Server: (join) session to join found %s\n", session_to_join->session_id);

    struct user_t* user = find_user(message->source, active_users);

    if (user == NULL) {
        printf("Server: error: user is not logged in\n");
        Message m;
        m.type = JN_NAK;
        strcpy(m.data, "user is not logged in\n");

        deliver_message(&m, sender_fd);
        return;
    }

    if (session_to_join == NULL) {
        printf("Server: error: session doesn't exist\n");
        Message m;
        m.type = JN_NAK;
        strcpy(m.data, "session doesn't exist\n");
        deliver_message(&m, sender_fd);
        return;

    }

    //printf("cur session is: %s\n" ,user->cur_session);
    if (strcmp(user->cur_session, "") != 0) {
        printf("Server: error: user has already joined session %s\n", user->cur_session);
        Message m;
        m.type = JN_NAK;
        strcpy(m.data, "user has already joined session\n");
        deliver_message(&m, sender_fd);
return;
    }

           
    strcpy(user->cur_session, message->data); //update user session info
        Message m;
        m.type = JN_ACK;
        deliver_message(&m, sender_fd);
}

void leave_s(struct lab3message* message, int sender_fd) {
    struct session_t* session_to_leave = find_session(message->data, active_sessions);
    struct user_t* user = find_user(message->source, active_users);

    if (user == NULL) {
        printf("Server: error: user is not logged in\n");
    }

    if (session_to_leave == NULL) {
        printf("Server: error: session doesn't exist\n");
        return;
    }

    //check if user is in the session that it wants to leave
    if ( strcmp(user->cur_session, message->data) != 0) {
        printf("Server: error: user is not even in this session\n");
        return;
    }

    //delete user from session
    //update user session info
    printf("User %s has left session %s\n", user->client_id, user->cur_session);
    strcpy(user->cur_session, "");
    printf(" %s\n", user->cur_session);
}

void create_s(struct lab3message* message, int sender_fd) {

    printf("------------CREATE SESSION --------------\n");
    //printf("Server: active session head %s\n", active_sessions->session_id);
    struct session_t* session_to_leave = find_session(message->data, active_sessions);
    
    if (session_to_leave != NULL) {
        printf("Server: error: session already exists\n");
        
        Message m;
        m.type = NS_NCK;
        strcpy(m.data, "Session already exists\n");
        
        deliver_message(&m, sender_fd); 
        return;
    }
    
    if (session_to_leave == NULL) {
        printf("Server: session dne creating...\n");
    }
    
    Session_t* new_session = (Session_t*) malloc(sizeof(Session_t));
    strcpy(new_session->session_id, message->data);
   // new_session->joined_users = NULL;
    new_session->next=NULL;

    active_sessions = add_session(new_session, active_sessions);
   // printf("Server: addsession %s\n", active_sessions->session_id);

    // Send a confirmation
    Message m;
    m.type = NS_ACK;
    deliver_message(&m, sender_fd);
}

void text_s(struct lab3message* message, int sender_fd) {
    printf("------------ MESSAGE --------------\n");
    struct user_t* user = find_user(message->source, active_users);

    if (user == NULL) {
        printf("Server: error: user is not logged in\n");
        return;
    }

    if ( strcmp(user->cur_session, "") == 0 ) {
        printf("Server: error: user has not joined a session\n");
        return;
    }

    //broadcast
    struct lab3message packet;
    packet.type = MESSAGE;
    strcpy(packet.data, message->data);
    packet.size = sizeof (message->data);
    strcpy(packet.source, message->source);

    struct user_t* cur = active_users;
    while (cur != NULL) {
        if ( (strcmp(user->cur_session, cur->cur_session)==0 )&& (strcmp(cur->client_id, message->source) != 0)) { //deliver message to everyone in session but user that sent message
			printf("send to socket: %d, client: %s\n", cur->sockfd, cur->client_id);
            deliver_message(&packet, cur->sockfd);
        }
        cur = cur->next;
    }
}

void query_s(struct lab3message* message, int sender_fd) {
    char users[MAXBUFLEN];
    char sessions[MAXBUFLEN];
    
    char user_title [MAXBUFLEN];
    strcpy(user_title, "-----------ACTIVE USERS------------\n");
    
    print_users(users, active_users);
    strcat(user_title, users);
   // printf("%s\n", user_title);
    
    char session_title [MAXBUFLEN];
    strcpy(session_title, "\n-----------ACTIVE SESSIONS----------\n");
    print_sessions(sessions, active_sessions);
    strcat(session_title, sessions);
    //printf("%s\n", session_title);
    
    //char data[MAXBUFLEN];
    strcat(user_title, session_title);
    
    Message m;
    m.type = QU_ACK;
    m.size = sizeof(user_title);
    strcpy(m.data, user_title);
    deliver_message(&m, sender_fd);
}


void invite_s(struct lab3message* message, int sender_fd){
    printf("Invitee is: %s\n", message->data); 
    
    //First, find the user we wanna send the invite to 
    struct user_t* user = find_user(message->data, active_users);
    
    //If the user doesn't exist, send INV_NACK to the user. 
    if (user == NULL){
         Message m;
         m.type = INV_NACK;
         deliver_message(&m, sender_fd);
        return;
    }
   
    //send an ack to the user saying the user was found and invite is was sent
    Message m;
    m.type = INV_ACK;
    deliver_message(&m, sender_fd);
    
    
    //need to forward the message to the specified client 
    
    deliver_message(message,user->sockfd); 
    
    
    
    return; 
}