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

/************* session list functions ************/
struct session_t* add_session(struct session_t* new_session, struct session_t* head) { //inserts at beginning of list
    if(head == NULL) {
        head = new_session;
        return head;
    }
    
    new_session->next = head;
    return new_session;    
}
struct session_t* delete_session(int session_id, struct session_t* head){
    struct session_t* cur = head;
    struct session_t* pre = NULL;
    
    while(cur != NULL && (cur->session_id != session_id) ) {
        pre = cur;
        cur = cur->next;
    }
    
    if(cur == NULL) { return NULL; } 
    if(pre == NULL) { return cur->next; }
    
    pre->next = cur->next;
    cur->next = NULL;
    return head;
}

struct session_t* find_session(int session_id, struct session_t* head){
    struct session_t* cur = head;
    
    while(cur != NULL && (cur->session_id != session_id) ) {
        cur = cur->next;
    }
    
    if(cur == NULL) { return NULL; }
    return cur;
}

/************** user list functions ***************/
struct user_t* add_user(struct user_t* new_user, struct user_t* head){
    if(head == NULL) {
        head = new_user;
        return head;
    }
    
    new_user -> next = head;
    return new_user;   
}
struct user_t* delete_user(char* client_id, struct user_t* head) {
    struct user_t* cur = head;
    struct user_t* pre = NULL;
    
    while(cur != NULL && (strcmp(client_id, cur->client_id) != 0) ) {
        pre = cur;
        cur = cur->next;
    }
    
    if(cur == NULL) { return NULL; } 
    if(pre == NULL) { return cur->next; }
    
    pre->next = cur->next;
    cur->next = NULL;
    return head; 
}
struct user_t* find_user(char* client_id, struct user_t* head){
    struct user_t* cur = head;
    
    while(cur != NULL && (strcmp(client_id, cur->client_id) != 0)  ) {
        cur = cur->next;
    }
    
    if(cur == NULL) { return NULL; }
    return cur;
}