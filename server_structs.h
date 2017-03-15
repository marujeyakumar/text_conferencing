#ifndef SERVER_STRUCTS_H
#define	SERVER_STRUCTS_H

struct user_t{
    char* client_id;
    char* password;
    int cur_session;
    
    struct user_t* next;
};

struct session_t{
    int session_id;
    struct session_t* next;
    
};


/************* session list functions ************/
struct session_t* add_session(struct session_t* new_session, struct session_t* head);
struct session_t* delete_session(int session_id, struct session_t* head);
struct session_t* find_session(int session_id, struct session_t* head);

/************** user list functions ***************/
struct user_t* add_user(struct user_t* new_session, struct user_t* head);
struct user_t* delete_user(char* client_id, struct user_t* head);
struct user_t* find_user(char*  client_id, struct user_t* head);


#endif