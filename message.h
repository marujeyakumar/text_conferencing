#ifndef MESSAGE_H
#define MESSAGE_H

//Constants
#define MAXBUFLEN 1500

enum TYPE {
    LOGIN = 1,
    LO_ACK = 2,
    LO_NACK = 3,
    EXIT = 4,
    JOIN = 5,
    JN_ACK = 6,
    JN_NAK = 7,
    LEAVE_SESS = 8,
    NEW_SESS = 9,
    NS_NCK = 10,
    NS_ACK = 11,
    MESSAGE = 12,
    QUERY = 13,
    QU_ACK = 14
};

typedef struct lab3message {
    unsigned int type;
    unsigned int size;
    unsigned char source[MAXBUFLEN];
    unsigned char data[MAXBUFLEN];
} Message;

int deliver_message(struct lab3message* message, int sockfd);
int receive_message(struct lab3message* message, int sockfd);

#endif
