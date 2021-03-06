#define CLIENT_PRINT

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

#include "client.h"
fd_set master, readfds;

int main(int argc, char *argv[]) {
    int fdmax;
    if (argc != 1) {
        fprintf(stderr, "program doesn't accept command line arguments\n");
    }
    printf("Client: Client Started\n");

    status.logged_in = 0;



    while (1) {
        //readfds = master; //make copy of master
        FD_ZERO(&readfds);
        FD_SET(fileno(stdin), &readfds);

        if (status.logged_in == 0) { //not logged in, only listen on stdin
            if (select(fileno(stdin) + 1, &readfds, NULL, NULL, NULL) == -1) {
                perror("Client: select");
                exit(1);
            }
        } else { //loggedin, so need to listen to sockfd+1
            FD_SET(status.sockfd, &readfds);
            if (select(status.sockfd + 1, &readfds, NULL, NULL, NULL) == -1) {
                perror("Client: select");
                exit(1);
            }
        }


        if (FD_ISSET(fileno(stdin), &readfds)) {
            int quit = handle_commands_c();
            if (quit == 1) break;

        } else if (FD_ISSET(status.sockfd, &readfds)) {

            struct lab3message recv_packet;
            receive_message(&recv_packet, status.sockfd);
            if (recv_packet.type == MESSAGE) {
                display_message(recv_packet);
            } else if (recv_packet.type == INVITE) {
                respond_to_invite(recv_packet.source, recv_packet.from);
            } else if (recv_packet.type == JN_ACK) {
                notify_client_that_invite_accepted();
            } else if (recv_packet.type == JN_NAK) {
                notify_client_that_invite_rejected();
            } else if (recv_packet.type == KICK_ACK) {
                printf("Let user know we're kicking them out\n");
                notify_client_that_kicked_out(recv_packet.from);
            }


        }

    }

} //end of main 

int handle_commands_c() {

    char command[MAXBUFLEN];
    char tmp[MAXBUFLEN];
    int rv = 0;
    fgets(command, 100, stdin);


    //checking to see if the user sent a message 
    if (command[0] != '/') {
        text_c(command);
        return 0;
    }

    //get the command type
    sscanf(command, "%s", tmp);
    int cmd_type = check_command(tmp);

    //based on that command type, do different things 
    if (cmd_type == -1) {
        printf("Invalid command plz try again\n");
        return 0;
    }

    if (cmd_type == 0) { //login 
        //we be logging in 
        char client_id[MAXBUFLEN];
        char password[MAXBUFLEN];
        char server_ip[MAXBUFLEN];
        int port;

        sscanf(command, "%s %s %s %s %d", tmp, client_id, password, server_ip, &port);
        login_c(client_id, password, server_ip, port);
    } else if (cmd_type == 1) { //logout
        logout();

    } else if (cmd_type == 2) { //join session 
        char session_id[MAXBUFLEN];
        sscanf(command, " %s %s", tmp, session_id);
        join_session(session_id);

    } else if (cmd_type == 3) { //leave session
        //leave a session 
        leave_session();
    } else if (cmd_type == 4) { //create session
        char session_id[MAXBUFLEN];
        sscanf(command, "%s %s", tmp, session_id);
        create_session(session_id);

    } else if (cmd_type == 5) { //list sessions
        list();
    } else if (cmd_type == 6) { //quit 
        quit();
        return 1;
    } else if (cmd_type == 7) {
        char invitee[MAXBUFLEN];
        sscanf(command, "%s %s", tmp, invitee);
        send_invite(invitee);
    } else if (cmd_type == 8) {
        char user_kicked[MAXBUFLEN];
        sscanf(command, "%s %s", tmp, user_kicked);
        kick_out_user(user_kicked);
    }

    return 0;
}

// get sockaddr, IPv4 or IPv6

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

//Check the Client command 

int check_command(char* command) {

    if (strcmp(command, "/login") == 0) return 0;
    if (strcmp(command, "/logout") == 0) return 1;
    if (strcmp(command, "/joinsession") == 0) return 2;
    if (strcmp(command, "/leavesession") == 0) return 3;
    if (strcmp(command, "/createsession") == 0) return 4;
    if (strcmp(command, "/list") == 0) return 5;
    if (strcmp(command, "/quit") == 0) return 6;
    if (strcmp(command, "/invite") == 0) return 7;
    if (strcmp(command, "/kickout") == 0) return 8;
    else return -1;
}

/***********************Functions for each command ******************************/

void login_c(char client_id[MAXBUFLEN], char password[MAXBUFLEN], char server_ip[MAXBUFLEN], int port) {
    printf("------- LOGIN ------\n");

    if (status.logged_in == 1) {
        printf("Login failure: you're already logged in!\n");
        return;
    }

    int sockfd, numbytes, rv;
    char s[INET6_ADDRSTRLEN];
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    char port_s[MAXBUFLEN];
    sprintf(port_s, "%d", port);

    //Error checking - if the server is incorrect, or if the port number was incorrect 
    if (strcmp(server_ip, "localhost") != 0) {
        printf("Login failure: Incorrect server ip was given.\n");
        return;
    }
    if (port != 5676) {
        printf("Login failure: Incorrect port number was given.\n");
        return;
    }
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
        printf("Client: client: failed to connect\n");
        exit(1);
    }

    /**************************** connected **********************************/

    status.p = p;
    status.sockfd = sockfd;
    strcpy(status.client_id, client_id);

    strcpy(status.password, password);

    struct lab3message packet;
    struct lab3message recv_packet;
    char data[MAXBUFLEN];
    sprintf(data, "%s %s", status.client_id, status.password);
    packet.type = LOGIN;
    packet.size = sizeof (data);
    strcpy(packet.source, status.client_id);
    strcpy(packet.data, data);

    deliver_message(&packet, status.sockfd);

    receive_message(&recv_packet, status.sockfd);

    if (recv_packet.type == LO_ACK) {
        printf("Client: Login Successful\n");
        status.logged_in = 1;

    } else {
        printf("Client: Login Unsuccessful\n");
    }
}

void logout() {
    printf("------- LOGOUT ------\n");
    if (status.logged_in == 0) {
        printf("Logout Error: You were not logged in before! \n");
        return;
    }
    Message packet;
    packet.type = EXIT;
    sprintf(packet.source, status.client_id);
    deliver_message(&packet, status.sockfd);
    status.logged_in = 0;
    close(status.sockfd);
    printf("Logged out Succesfully.\n");
    // FD_CLR(status.sockfd, &readfds);
}

void join_session(char session_id[MAXBUFLEN]) {
    printf("------- JOIN SESSION ------\n");
    if (status.logged_in == 0) {
        printf("Join Session Error: You were not logged in before! \n");
        return;
    }

    //Construct packet to send 
    struct lab3message packet, recv_packet;
    char data[MAXBUFLEN];
    sprintf(data, "%s", session_id);
    packet.type = JOIN; //to indicate we are creating a new session
    packet.size = sizeof (data);
    strcpy(packet.source, status.client_id);
    strcpy(packet.data, data);

    deliver_message(&packet, status.sockfd);

    receive_message(&recv_packet, status.sockfd);

    if (recv_packet.type == JN_ACK) {
        strcpy(status.session_id, session_id);
        printf("Client: Successfully joined session %s\n", session_id);

    } else {
        printf("Join session failure: Unable to join session\n");
    }

}

void leave_session() {

    printf("------- LEAVE SESSION ------\n");

    if (status.logged_in == 0) {
        printf("Leave Session Error: You were not logged in before! \n");
        return;
    }
    if (strcmp(status.session_id, "") == 0) {
        printf("Leave Session failure: User is has not joined any sessions\n");
        return;
    }

    Message packet;
    packet.type = LEAVE_SESS; //leave session 
    sprintf(packet.source, status.client_id);
    sprintf(packet.data, status.session_id);
    deliver_message(&packet, status.sockfd);

    Message recv_packet;
    receive_message(&recv_packet, status.sockfd);
    if (recv_packet.type == LEAVE_ACK) {
        printf("You have succesfully left the session!\n");
    }
    if (recv_packet.type == LEAVE_NACK) {
        printf("Leave session was not succesful. Either you dont exit, are not part of a session, or the session to leave doesn't exist.\n");
    }

}

void create_session(char session_id[MAXBUFLEN]) {
    printf("------- CREATE SESSION ------\n");

    if (status.logged_in == 0) {
        printf("Create Session Error: You were not logged in before! \n");
        return;
    }

    //put the necessary information into the packet 
    struct lab3message packet, recv_packet;
    char data[MAXBUFLEN];
    sprintf(data, "%s", session_id);
    packet.type = NEW_SESS; //to indicate we are creating a new session
    packet.size = sizeof (data);
    strcpy(packet.source, status.client_id);
    strcpy(packet.data, data);

    deliver_message(&packet, status.sockfd);

    receive_message(&recv_packet, status.sockfd);

    if (recv_packet.type == NS_ACK) {
        strcpy(status.session_id, session_id);
        printf("Client: Successfully created session %s\n", session_id);
    } else {
        printf("Client: Unable to create session\n");
    }
}

void list() {
    printf("------- LIST ------\n");

    //put the necessary information into the packet 
    struct lab3message packet, recv_packet;

    //to indicate to the server we would like to query 
    packet.type = QUERY;
    strcpy(packet.source, status.client_id);

    deliver_message(&packet, status.sockfd);

    receive_message(&recv_packet, status.sockfd);
    printf("%s\n", recv_packet.data);
}

void quit() {
    printf("------- QUIT------\n");
    printf("Quit: user has successfully quit session. Goodbye\n");
    close(status.sockfd);
    exit(1);
}

void text_c(char text[MAXBUFLEN]) {
    printf("------- MESSAGE ------\n");

    if (status.logged_in == 0) {
        printf("Message send failure: user is not logged in\n");
        return;
    }
    if (strcmp(status.session_id, "") == 0) {
        printf("Message send failure: user is not part of a session! Please join a session to send messages.\n");
        return;
    }

    //put the necessary information into the packet 
    struct lab3message packet, recv_packet;
    char data[MAXBUFLEN];
    sprintf(data, "%s", text);
    packet.type = MESSAGE; //to indicate we are creating a new session
    packet.size = sizeof (data);
    strcpy(packet.source, status.client_id);
    strcpy(packet.data, data);

    deliver_message(&packet, status.sockfd);
    printf("Message successfully sent!\n");
}

void send_invite(char invitee[MAXBUFLEN]) {
    printf("------- SEND INVITE ------\n");
    if (status.logged_in == 0) {
        printf("Send invite failure: user is not logged in\n");
        return;
    }
    if (strcmp(status.session_id, "") == 0) {
        printf("Send invite failure: user is not part of a session!\n");
        return;
    }

    //send a message to the server to make sure the user we're sending to actually exists
    struct lab3message packet, recv_packet;
    char data[MAXBUFLEN];
    sprintf(data, "%s", invitee);
    packet.type = INVITE; //to indicate we are creating a new session
    packet.size = sizeof (data);
    //send the session id so the client on the other side knows which session to join right??? 
    strcpy(packet.source, status.session_id);
    strcpy(packet.data, data);
    //also need to keep track of WHO sent the invite in the first place 
    strcpy(packet.from, status.client_id);
    deliver_message(&packet, status.sockfd);

    //Now we receive message back from the server on the status of our invite 
    receive_message(&recv_packet, status.sockfd);
    //Error checking - if the user even exists
    if (recv_packet.type == INV_NACK) {
        printf("Send Invite Error: Invitee does not exist! \n");
        return;
    }
    if (recv_packet.type == INV_ACK) {
        printf("Send Invite Success: User was found - invite has been sent!\n");
    }



}

void respond_to_invite(char session[MAXBUFLEN], char inviter[MAXBUFLEN]) {
    printf("-------------- RECEIVED INVITE-----------------\n");
    printf("You have received an invite to join session %s\n", session);
    printf("Would you like to join? (y/n)\n");

    //used a simple while loop to manage the user input 
    char response[MAXBUFLEN];
    while (1) {

        fgets(response, 100, stdin);

        //send a message with INV_Y to accept the invite
        if (strcmp(response, "y\n") == 0) {
            printf("Congrats! You have sucessfully joined session %s!\n", session);

            struct lab3message packet, recv_packet;
            char data[MAXBUFLEN];
            sprintf(data, "%s", status.client_id);
            packet.type = INV_Y; //to indicate we are agreeing to join the session
            packet.size = sizeof (data);

            //send the session id so the client on the other side knows which session to join right??? 
            strcpy(packet.source, session);
            strcpy(packet.data, data);
            strcpy(packet.from, inviter);
            deliver_message(&packet, status.sockfd);

            break;

        } else if (strcmp(response, "n\n") == 0) {
            printf("Rejection sucessfully sent. Sorry to hear that you don't want to join session %s!\n", session);

            struct lab3message packet, recv_packet;
            char data[MAXBUFLEN];
            sprintf(data, "%s", status.client_id);
            packet.type = INV_N; //to indicate we have declined the invite     
            strcpy(packet.from, inviter);
            deliver_message(&packet, status.sockfd);

            break;

        } else {
            printf("Wrong input, please enter y to accept or n to reject. \n");
            continue;
        }
    }

    return;
}

void notify_client_that_invite_accepted() {

    printf("-----------INVITE RESPONSE-----------\n");
    printf("Success! Your invitation was accepted.\n");
    return;
}

void notify_client_that_invite_rejected() {
    printf("-----------INVITE RESPONSE-----------\n");
    printf("Unfortunately, your invite was not accepted.\n");
    return;
}

void display_message(struct lab3message packet) {
    printf("------------YOU HAVE RECEIVED A MESSAGE-----------\n");
    printf("Message from %s: %s\n", packet.source, packet.data);
    return;
}

void kick_out_user(char user_kicked[MAXBUFLEN]) {
    printf("------------KICK OUT USER-----------\n");
    if (status.logged_in == 0) {
        printf("Kick Out failure: user is not logged in\n");
        return;
    }
    if (strcmp(status.session_id, "") == 0) {
        printf("Kick Out Failure: user is not part of a session!\n");
        return;
    }
    if (strcmp(status.client_id, user_kicked) == 0) {
        printf("Kick Out Failure: you can't kick yourself out!\n");
        return;
    }

    struct lab3message packet, recv_packet;
    char data[MAXBUFLEN];
    sprintf(data, "%s", user_kicked);
    packet.type = KICK_OUT; //to indicate we are creating a new session
    packet.size = sizeof (data);
    //send the session id so the client on the other side knows which session to search this user in. 
    strcpy(packet.source, status.session_id);
    strcpy(packet.data, data);
    //also need to keep track of WHO sent the invite in the first place 
    strcpy(packet.from, status.client_id);
    deliver_message(&packet, status.sockfd);


    receive_message(&recv_packet, status.sockfd);
    if (recv_packet.type == KICK_ACK) {
        printf("You have successfully kicked out %s\n", user_kicked);
        return;
    } else if (recv_packet.type == KICK_NAK) {
        printf("Kick out unsuccessful! Either the user doesn't exist or isn't part of your session.\n");
    }

}

void notify_client_that_kicked_out(char who_kicked[MAXBUFLEN]) {
    printf("-------------KICKED OUT NOTIFICATION-------------\n");
    printf("Unfortunately you have been kicked out of session %s by %s\n", status.session_id, who_kicked);
    return;
}