#ifndef MESSAGE_H
#define	MESSAGE_H

//Constants
#define MAXBUFLEN 1500

enum TYPE{
	LOGIN, 
	LO_ACK,
	LO_NACK,
	EXIT,
	JOIN, 
	JN_ACK,
	JN_NACK,
	LEAVE_SESS,
	NEW_SESS,
	NS_ACK,
	MESSAGE, 
	QUERY, 
	QU_ACK
};

struct lab3message {
	unsigned int type;
	unsigned int size;
	unsigned char source[MAXBUFLEN];
	unsigned char data[MAXBUFLEN];
};

int deliver_message(struct lab3message* message, int sockfd);
int receive_message(struct lab3message* message, int sockfd);

#endif
