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

int deliver_message(struct lab3message* message, int sockfd){

	int numbytes=0;
	if ((numbytes = send(sockfd, message, sizeof(struct lab3message), 
								0)) == -1) {
			perror("client: sendto packet fail\n");
			exit(1);	
	}
        printf("delivered done \n");
	
}

int receive_message(struct lab3message* message, int sockfd){
	
	int numbytes=0;
	if ((numbytes = recv(sockfd, message, sizeof(struct lab3message), 
								0)) == -1) {
			perror("client: sendto packet fail\n");
			exit(1);	
	}
}

/*int handle_message_s(struct lab3message* message){
	switch(message -> type){
		case(LOGIN): login_s(message);
		case(EXIT): ;
		case(JOIN): ;
		case(LEAVE_SESS): ;
		case(NEW_SESS): ;
		case(MESSAGE): ;
		case(QUERY): ;
	}

}
*/
/*void login_s(lab3message* message){
    int valid_user_pw = 1; //add check later
    struct lab3message packet;
    
    if (valid_user_pw) {
        char* data = "login_ack";
        packet.type = LO_ACK;
        
    } else{
        char* data = "login_nack";
        packet.type = LO_NACK;
    }
    packet.size = sizeof (packet.data);
    strcpy(packet.source, 0);
    strcpy(packet.data, packet.data);
    deliver_message(&packet, newfd);
}*/

/*int handle_message_c(struct lab3message* message){
	switch(message -> type){
		case(LO_ACK): ;
		case(LO_NACK): ;
		case(JN_ACK): ;
		case(JN_NACK): ;
		case(NS_ACK): ;
		case(MESSAGE): ;
                case(QU_ACK): ;
	}

}*/