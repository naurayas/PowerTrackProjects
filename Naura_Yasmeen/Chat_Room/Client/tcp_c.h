#ifndef TCP_H
#define TCP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 6333

#define MAX_CLIENTS         10

#pragma pack(1)

// client states
typedef enum
{
    MENU = 1,
    REGISTER,
    LOGIN,
    CHAT,
    ONLINE_USERS,
    MULTI_CHAT,
    SINGLE_CHAT,
    USER_LEFT,
    LOGOUT
}tcp_state;

// status
typedef enum
{
    SUCCESS = 1,
    FAILURE,
    DUPLICATE_USER,
    USER_NOT_FOUND,
    PASSWORD_MISMATCH,
}tcp_status;

// packet structure to share with server
typedef struct 
{
    tcp_state state;
    tcp_status status;

    struct 
    {
        char username[32];
        char password[32];
    }user;

    struct 
    {
        char sender[32];
        char receiver[32];
        char message[256];
    }chat;

    // structure to get the online users list
    struct
    {
        int count;
        char users[MAX_CLIENTS][32];
    }online;
    
}packet_t;
int register_user(packet_t *pkt, int sockfd);
int login_user(packet_t *pkt, int sockfd);
void handle_chatopt(packet_t *pkt, int sockfd);
void single_chat(packet_t *pkt, int sockfd);
void signal_handler(int signum);
void multi_chat(packet_t *pkt, int sockfd);

#endif
