#ifndef MESSAGE_H
#define MESSAGE_H

//Constants
#define MAXBUFLEN 1500

enum TYPE {
    LOGIN      = 1,
    LO_ACK     = 2,
    LO_NACK    = 3,
    EXIT       = 4,
    JOIN       = 5,
    JN_ACK     = 6,
    JN_NAK     = 7,
    LEAVE_SESS = 8,
    LEAVE_ACK  = 9,
    LEAVE_NACK = 10,
    NEW_SESS   = 11,
    NS_NCK     = 12,
    NS_ACK     = 13,
    MESSAGE    = 14,
    QUERY      = 15,
    QU_ACK     = 16, 
    INVITE     = 17,
    INV_ACK    = 18,
    INV_NACK   = 19,
    INV_Y      = 20, 
    INV_N      = 21
};

typedef struct lab3message {
    unsigned int type;
    unsigned int size;
    unsigned char source[MAXBUFLEN];
    unsigned char data[MAXBUFLEN];
    unsigned char from[MAXBUFLEN];
} Message;

int deliver_message(struct lab3message* message, int sockfd);
int receive_message(struct lab3message* message, int sockfd);

#endif
