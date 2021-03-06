#include "message.h"

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

int deliver_message(struct lab3message* message, int sockfd) {
    //printf("Delivering message %o ...", message->type);
    
    int numbytes = 0;
    if ((numbytes = send(sockfd, message, sizeof (struct lab3message),
            0)) == -1) {
        perror("client: sendto packet fail\n");
        exit(1);
    }
  // printf("Delivered\n");
}

int receive_message(struct lab3message* message, int sockfd) {
  //  printf("Receiving message ...");
    
    int numbytes = 0;
    if ((numbytes = recv(sockfd, message, sizeof (struct lab3message),
            0)) == -1) {
        perror("client: recieved packet fail\n");
        exit(1);
    }
    
  //  printf("Recieved %o\n", message->type);
}

