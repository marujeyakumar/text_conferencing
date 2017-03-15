
#include "client.h"

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



int main (int argc, char *argv[]) {

    if (argc != 1) {
        fprintf(stderr, "program doesn't accept command line arguments\n");
    }

    char command[MAXBUFLEN];
    //get command from the client.
    while (1) {
        scanf("%s", command);

        //get the command type
        int cmd_type = check_command(command);

        //based on that command type, do different things 
        if (cmd_type == LOG_IN) {
            char client_id[MAXBUFLEN];
            char password[MAXBUFLEN];
            char server_ip[MAXBUFLEN];
            int port;

            scanf("%s %s %s %d", client_id, password, server_ip, &port);
            login_c(client_id, password, server_ip, port);
        } else if (cmd_type == LOG_OUT) {
        } else if (cmd_type == JOIN_SESSION) {
        } else if (cmd_type == LEAVE_SESSION) {
        } else if (cmd_type == CREATE_SESSION) {
        } else if (cmd_type == LIST) {
        } else if (cmd_type == QUIT) {
        }

    }

} //end of main 




// get sockaddr, IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa) {
 if (sa->sa_family == AF_INET) {
 return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

//Check the Client command 

int check_command(char* command) {

    if (strcmp(command, "/login") == 0) return LOG_IN;
    if (strcmp(command, "/logout") == 0) return LOG_OUT;
    if (strcmp(command, "/joinsession") == 0) return JOIN_SESSION;
    if (strcmp(command, "/leavesession") == 0) return LEAVE_SESSION;
    if (strcmp(command, "/createsession") == 0) return CREATE_SESSION;
    if (strcmp(command, "/list") == 0) return LIST;
    if (strcmp(command, "/quit") == 0) return QUIT;


    //defualt case 
    return -1;
}

/***********************Functions for each command ******************************/

void login_c(char client_id[MAXBUFLEN], char password[MAXBUFLEN], char server_ip[MAXBUFLEN], int port) {

    int sockfd, numbytes, rv;
    char s[INET6_ADDRSTRLEN];
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    char port_s[MAXBUFLEN];
    sprintf(port_s, "%d", port);

    if ((rv = getaddrinfo(server_ip, port_s, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket: ");
            continue;
        }
        if ((connect(sockfd, p->ai_addr, p->ai_addrlen)) == -1) {
            close(sockfd);
            perror("client: connect: ");
            continue;
        }
        break;
    }

    if (p == NULL) {
        printf("client: failed to connect\n");
        exit(1);
    }

    /**************************** connected **********************************/

    status.p = p;
    status.sockfd = sockfd;
    strcpy(status.client_id, client_id);

    strcpy(status.password, password);

    struct lab3message packet;
    char data[MAXBUFLEN];
    sprintf(data, "%s %s", status.client_id, status.password);
    packet.type = LOGIN;
    packet.size = sizeof (data);
    strcpy(packet.source, status.client_id);
    strcpy(packet.data, data);

    deliver_message(&packet, status.sockfd);

    receive_message(&packet, status.sockfd);
    printf("received: %s\n", packet.data);
}
void logout(){}
void join_session(){}
void leave_session(){}
void create_session(){}
void list(){}
void quit(){}
