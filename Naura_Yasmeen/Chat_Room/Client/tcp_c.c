#include "tcp_c.h"

volatile int leave_chat = 0;

void signal_handler(int signum)
{
    // leave the chat
    leave_chat = 1;
}

int register_user(packet_t *pkt, int sockfd)
{
    // send the user details to the server
    if(send(sockfd, pkt, sizeof(packet_t), 0) == -1)
    {
        perror("send");
        return FAILURE;
    }

    // receive registration status from server
    if(recv(sockfd, pkt, sizeof(packet_t), 0) == -1)
    {
        perror("recv");
        return FAILURE;
    }
    
    // check the status of registration
    switch(pkt->status)
    {
        case SUCCESS:
            printf("\nINFO: User registration successful\n");
            return SUCCESS;
        case DUPLICATE_USER:
            printf("\nINFO: User already registered\n");
            return FAILURE;
    }

    printf("\nINFO: User Registration failed\n");
    return FAILURE;
}

int login_user(packet_t *pkt, int sockfd) 
{
    // send the user details to the server
    if(send(sockfd, pkt, sizeof(packet_t), 0) == -1)
    {
        perror("send");
        return FAILURE;
    }

    // receive login status from server
    if(recv(sockfd, pkt, sizeof(packet_t), 0) == -1)
    {
        perror("recv");
        return FAILURE;
    }

    // check the status of login
    switch(pkt->status)
    {
        case SUCCESS:
            printf("\nINFO: User login successful\n");
            return SUCCESS;
        case USER_NOT_FOUND:
            printf("\nINFO: User is not registered\n");
            return FAILURE;
        case PASSWORD_MISMATCH:
            printf("\nINFO: User password does not match\n");
            return FAILURE;
    }

    printf("\nINFO: User login failed\n");
    return FAILURE;
}

void handle_chatopt(packet_t *pkt, int sockfd)
{
    // print chat options
    printf("\nChat Options:\n");
    printf("1. Single User Chat\n");
    printf("2. Multi User Chat\n");
    printf("3. Logout\n");
    printf("\nSelect what you would like to proceed with:");
    int opt;
    scanf("%d", &opt);
    printf("\n");

    switch(opt)
    {
        case 1:
            // get online users list from the server
            // send request to get the online users
            pkt->state = ONLINE_USERS;
            if(send(sockfd, pkt, sizeof(packet_t), 0) == -1)
            {
                perror("send");
                pkt->state = CHAT;
                return;
            }
            // receive the list from server
            packet_t recvpkt;
            memset(&recvpkt, 0, sizeof(recvpkt));

            if(recv(sockfd, &recvpkt, sizeof(packet_t), 0) == -1)
            {
                perror("recv");
                pkt->state = CHAT;
                return;
            }
            // validate there are other online users
            if(recvpkt.online.count == 0)
            {
                printf("\nNo user is online\n");
                pkt->state = CHAT;
                return;
            }
            for (int i = 0; i < recvpkt.online.count; i++)
            {
                printf("%d. %s\n", i + 1, recvpkt.online.users[i]);
            }
            int choice;
            printf("Select a user: ");
            scanf("%d", &choice);
            // validate choice
            if(choice > recvpkt.online.count)
            {
                printf("\nInvalid choice!\n");
                pkt->state = CHAT;
                return;
            }
            // copy the selected user to the reciever field
            strcpy(pkt->chat.receiver, recvpkt.online.users[choice - 1]);
            // change state to SINGLE_CHAT
            pkt->state = SINGLE_CHAT;
            break;
        case 2:
            // change state to MULTI_CHAT
            pkt->state = MULTI_CHAT;
            break;
        case 3:
            // change state to LOGOUT
            pkt->state = LOGOUT;
            break;
        default:
            printf("Invalid Option!\n");
    }
}

void single_chat(packet_t *pkt, int sockfd)
{
    strcpy(pkt->chat.sender, pkt->user.username);
    // select() monitors keyboard ready (user typed) and socket ready (message arrived)
    fd_set readfds;
    int maxfd = sockfd + 1;

    // implement single chat
    while(1)
    {
        // handle leave_chat signal
        if(leave_chat)
        {
            // change state to CHAT (let server know client left)
            pkt->state = CHAT;
            // inform server
            if(send(sockfd, pkt, sizeof(packet_t), 0) == -1)
            {
                perror("send");
                continue;
            }
            leave_chat = 0;
            break;
        }

        // clear set
        FD_ZERO(&readfds);
        // add the fd to monitor
        FD_SET(0, &readfds);
        FD_SET(sockfd, &readfds);

        if(select(maxfd, &readfds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            continue;
        }

        // check if keyboard is ready (outgoing message)
        if(FD_ISSET(0, &readfds))
        {
            // read message
            scanf(" %255[^\n]", pkt->chat.message);
            // send SINGLE_CHAT packet to server with the message to forward
            if(send(sockfd, pkt, sizeof(packet_t), 0) == -1)
            {
                perror("send");
                continue;
            }
        }

        // check if socket is ready (incoming message)
        if(FD_ISSET(sockfd, &readfds))
        {
            // recieve the message packet from the other client
            packet_t recvpkt;
            memset(&recvpkt, 0, sizeof(recvpkt));
            if(recv(sockfd, &recvpkt, sizeof(packet_t), 0) == -1)
            {
                perror("recv");
                continue;
            }
            // check if receiver left
            if(recvpkt.status == USER_LEFT)
            {
                printf("%s left the chat\n", recvpkt.chat.sender);
                pkt->state = CHAT;
                break;
            }
            // print the message
            printf("%s: %s\n", recvpkt.chat.sender, recvpkt.chat.message);
        }
    }
}

void multi_chat(packet_t *pkt, int sockfd)
{
    strcpy(pkt->chat.sender, pkt->user.username);
    // select() monitors keyboard ready (user typed) and socket ready (message arrived)
    fd_set readfds;
    int maxfd = sockfd + 1;

    // implement multi chat
    while(1)
    {
        // handle leave_chat signal
        if(leave_chat)
        {
            // change state to CHAT (let server know client left)
            pkt->state = CHAT;
            // inform server
            if(send(sockfd, pkt, sizeof(packet_t), 0) == -1)
            {
                perror("send");
                continue;
            }
            leave_chat = 0;
            break;
        }

        // clear set
        FD_ZERO(&readfds);
        // add the fd to monitor
        FD_SET(0, &readfds);
        FD_SET(sockfd, &readfds);

        if(select(maxfd, &readfds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            continue;
        }

        // check if keyboard is ready (outgoing message)
        if(FD_ISSET(0, &readfds))
        {
            // read message
            scanf(" %255[^\n]", pkt->chat.message);
            // send SINGLE_CHAT packet to server with the message to forward
            if(send(sockfd, pkt, sizeof(packet_t), 0) == -1)
            {
                perror("send");
                continue;
            }
        }

        // check if socket is ready (incoming message)
        if(FD_ISSET(sockfd, &readfds))
        {
            // recieve the message packet from the other client
            packet_t recvpkt;
            memset(&recvpkt, 0, sizeof(recvpkt));
            if(recv(sockfd, &recvpkt, sizeof(packet_t), 0) == -1)
            {
                perror("recv");
                continue;
            }
            // check if receiver left
            if(recvpkt.status == USER_LEFT)
            {
                printf("%s left the chat\n", recvpkt.chat.sender);
                continue;
            }
            // print the message
            printf("%s: %s\n", recvpkt.chat.sender, recvpkt.chat.message);
        }
    }
}