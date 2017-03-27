#ifndef SERVER_STRUCTS_H
#define	SERVER_STRUCTS_H

#define MAXBUFLEN 1500

typedef struct user_t{
    int sockfd;
    
    char client_id[MAXBUFLEN];
    char password[MAXBUFLEN];
    
    char cur_session[MAXBUFLEN];
    
    struct user_t* next;
} User_t;

typedef struct session_t{
    char session_id[MAXBUFLEN];
   // struct user_t* joined_users;
    struct session_t* next;
} Session_t;

extern Session_t* active_sessions;
extern User_t* active_users;

/************* session list functions ************/
struct session_t* add_session(struct session_t* new_session, struct session_t* head);
struct session_t* delete_session(char* session_id, struct session_t* head);
struct session_t* find_session(char* session_id, struct session_t* head);
void print_sessions(char*sessions, struct session_t* head);

/************** user list functions ***************/
struct user_t* add_user(struct user_t* new_user, struct user_t* head);
struct user_t* delete_user(char* client_id, struct user_t* head);
struct user_t* find_user(char*  client_id, struct user_t* head);
void print_users(char*sessions, struct user_t* head);
#endif