/* Name         : Naura Yasmeen U
   Reg No.      : 25017_094
   Start Date   : 08/06/2026
   End Date     : 10/07/2026
   Project Name : TCP/IP Chat Room
*/
#include "tcp_s.h"

int main()
{
    // create a TCP IPv4 socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        perror("socket");
        return FAILURE;
    }

    // bind socket to ip and port
    struct sockaddr_in server_info;
    memset(&server_info, 0, sizeof(server_info));
    server_info.sin_family = AF_INET;
    server_info.sin_port = htons(PORT);
    server_info.sin_addr.s_addr = INADDR_ANY;

    if(bind(sockfd, (struct sockaddr *)&server_info, sizeof(server_info)) == -1)
    {
        perror("bind");
        return FAILURE;
    }

    printf("Chat Server: Port Information: 127.0.0.1:%d\n", PORT);

    // listen for client connections
    if(listen(sockfd, MAX_CLIENTS) == -1)
    {
        perror("listen");
        return FAILURE;
    }

    packet_t pkt;
    memset(&pkt, 0, sizeof(pkt));

    // initialize fd_set
    fd_set masterfds, readyfds;

    // clear the fd set
    FD_ZERO(&masterfds);
    FD_ZERO(&readyfds);

    // add socket fd to masterfds set
    FD_SET(sockfd, &masterfds);
    int maxfd = sockfd + 1;

    // initialize the client array
    client_t clients[MAX_CLIENTS];
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        clients[i].fd = -1;
        clients[i].logged_in = 0;
        memset(clients[i].username, 0, sizeof(clients[i].username));
    }

    while(1)
    {
        // select() modifies the fd set each time
        readyfds = masterfds;
        // monitor client connections, store into set
        if(select(maxfd, &readyfds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            continue;
        }
        // check if server fd ready 
        if(FD_ISSET(sockfd, &readyfds))
        {
            // accept new client
            struct sockaddr_in client_addr;
            memset(&client_addr, 0, sizeof(client_addr));
            socklen_t addr_len = sizeof(client_addr);
            int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);
            if (clientfd == -1)
            {
                perror("accept");
                return FAILURE;
            }
            // find free slots to accommodate clients
            int index = -1;
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if(clients[i].fd == -1)
                {
                    index = i;
                    break;
                }
            }
            if(index == -1)
            {
                printf("Maximum clients reached\n");
                close(clientfd);
            }
            else
            {
                // update the client details in the client array
                clients[index].fd = clientfd;
                clients[index].addr = client_addr;
                clients[index].logged_in = 0;
                // add new client fd to set
                FD_SET(clientfd, &masterfds);
                // update maxfd
                if(clientfd >= maxfd)
                    maxfd = clientfd + 1;
            }
        }

        // process each client
        for(int i = 0; i < maxfd; i++)
        {
            if(i != sockfd)
            {
                if(FD_ISSET(i, &readyfds))
                {
                    int ret;
                    // receive state option from client
                    if((ret = recv(i, &pkt, sizeof(pkt), 0)) == -1)
                    {
                        perror("recv");
                        continue;
                    }
                    // client disconnects
                    if(ret == 0)
                    {
                        close(i);
                        FD_CLR(i, &masterfds);
                        // clear the client details
                        for(int j = 0; j < MAX_CLIENTS; j++)
                        {
                            if(clients[j].fd == i)
                            {
                                clients[j].fd = -1;
                                clients[j].logged_in = 0;
                                memset(clients[j].username, 0, sizeof(clients[j].username));
                                break;
                            }
                        }
                    }
                    else
                    {
                        // map fd with clients list to get the ip and port
                        client_t *client = NULL;
                        for (int j = 0; j < MAX_CLIENTS; j++)
                        {
                            if(clients[j].fd == i)
                            {
                                client = &clients[j];
                                break;
                            }
                        }
                        if(client == NULL)
                        {
                            continue;
                        }
                        // check the state of client
                        switch(pkt.state)
                        {
                            case LOGIN:
                                printf("Chat Server: Incoming login request at %s:%d\n", inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));
                                handle_login(&pkt, client);
                                // send registration status to client
                                if(send(i, &pkt, sizeof(pkt), 0) == -1)
                                {
                                    perror("send");
                                    continue;
                                }
                                break;
                            case REGISTER:
                                printf("Chat Server: Incoming registration request at %s:%d\n", inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));
                                handle_register(&pkt, client);
                                // send registration status to client
                                if(send(i, &pkt, sizeof(pkt), 0) == -1)
                                {
                                    perror("send");
                                    continue;
                                }
                                break;
                            case ONLINE_USERS:
                                printf("Chat Server: Incoming online users list request at %s:%d\n", inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));
                                handle_online_rqt(&pkt, clients);
                                // send registration status to client
                                if(send(i, &pkt, sizeof(pkt), 0) == -1)
                                {
                                    perror("send");
                                    continue;
                                }
                                break;
                            case SINGLE_CHAT:
                                handle_single_chat(&pkt, clients);
                                break;
                            case MULTI_CHAT:
                                handle_multi_chat(&pkt, clients, i);
                                break;
                            case CHAT:
                                // client left single/multi chat
                                printf("Chat Server: %s left chat\n", pkt.chat.sender);
                                // update status and notify reciever
                                pkt.status = USER_LEFT;
                                if(strlen(pkt.chat.receiver) == 0)
                                {
                                    // left group chat
                                    handle_multi_chat(&pkt, clients, i);
                                }
                                else
                                {
                                    // left single chat
                                    handle_single_chat(&pkt, clients);
                                }
                                break;
                            case LOGOUT:
                                // client exiting
                                printf("Chat Server: %s is Offline\n", pkt.user.username);
                                handle_logout(&pkt, clients, i, &masterfds);
                                break;
                        }
                    }
                }
            }
        } 
    }

    return SUCCESS;
}