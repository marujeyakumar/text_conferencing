#include "server.h"
#include "server_structs.h"

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

int newfd;
struct addrinfo *p;
struct sockaddr_storage their_addr; // connector's address information

struct session_t* active_sessions = NULL;
struct user_t* active_users = NULL;

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("usage: server server_portnumber\n");
        exit(1);
    }

    int sockfd = start_server(argv[1]);
    listen_server(sockfd);
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
    int numbytes, i;
    struct lab3message new_mess;

    fd_set activefds, readfds;

    FD_ZERO(&activefds);
    FD_SET(sockfd, &activefds);


    while (1) {
        readfds = activefds; //update readfds 
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
                    printf("New connection at sockfd %d\n", newfd);
                }

                receive_message(&new_mess, newfd);
                printf("%d %d %s %s\n", new_mess.type, new_mess.size, new_mess.source, new_mess.data);
                handle_message_s(&new_mess);
            }
        }

    }

}

void handle_message_s(struct lab3message* message){
    switch(message -> type){
            case(LOGIN): 
                login_s(message);
                break;
            case(EXIT): 
                exit_s();
                break;
            case(JOIN): 
                join_s(message);
                break;
            case(LEAVE_SESS): ;
            case(NEW_SESS): ;
            case(MESSAGE): ;
            case(QUERY): ;
        default: printf("Client send invalid packet type\n");
    } 
}

int check_user(char* id, char* pw){
    if( strcmp(id, "jean")==0 ) {
        if(strcmp(pw, "pw")==0){
            return 0;
        } else return 1; //bad pw
    } 
    return 2; //no user
}

void login_s(struct lab3message* message){
    char *token;
    char client_id [MAXBUFLEN];
    char pw[MAXBUFLEN]; 
    
    token = strtok(message->data, " ");
    strcpy(client_id, token);
    token = strtok(NULL, "");
    strcpy(pw, token);

    int valid_user_pw = check_user(client_id, pw); 
    struct lab3message packet;
    char* data;
    
    if (valid_user_pw == 0) {
        data = "";
        packet.type = LO_ACK;
        
        /*struct user_t new_user =  
        active_users = add_user();*/
    } else if (valid_user_pw == 1){
        data = "incorrect password";
        packet.type = LO_NACK;
    } else if(valid_user_pw == 2) {
        data = "user does not exist";
        packet.type = LO_NACK;
    }
    packet.size = sizeof (packet.data);
    strcpy(packet.data, data);

    deliver_message(&packet, newfd);
}

void exit_s(){
    //set user sess to false
    //close socket?
}

void join_s(struct lab3message* message){
    
}