/* Name         : Naura Yasmeen U
   Reg No.      : 25017_094
   Start Date   : 08/06/2026
   End Date     : 10/07/2026
   Project Name : TCP/IP Chat Room
*/

#include "tcp_c.h"

int main()
{
    // create a TCP IPv4 socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        perror("socket");
        return FAILURE;
    }

    // connect to the server
    struct sockaddr_in server_info;
    memset(&server_info, 0, sizeof(server_info));
    server_info.sin_family = AF_INET;
    server_info.sin_port = htons(PORT);
    server_info.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(sockfd, (struct sockaddr *)&server_info, sizeof(server_info)) == -1)
    {
        perror("connect");
        return FAILURE;
    }
    printf("INFO: Connected to the server\n");

    int choice, ret;
    packet_t pkt;
    memset(&pkt, 0, sizeof(pkt));

    pkt.state = MENU;

    while(1)
    {
        
        switch(pkt.state)
        {
            case MENU:
                printf("\n1. Login\n");
                printf("2. Register\n");
                printf("3. Exit\n");
                printf("\nEnter your option: ");
                scanf("%d", &choice);
                switch(choice)
                {
                    case 1:
                        pkt.state = LOGIN;
                        break;
                    case 2:
                        pkt.state = REGISTER;
                        break;
                    case 3:
                        pkt.state = LOGOUT;
                        break;
                    default:
                        printf("Invalid option\n");
                }
                break;
            case LOGIN:
                printf("\nEnter User Name: ");
                scanf(" %31[^\n]", pkt.user.username);
                printf("Enter Password: ");
                scanf(" %31[^\n]", pkt.user.password);
                ret = login_user(&pkt, sockfd);
                if(ret == SUCCESS)
                {
                    pkt.state = CHAT;
                }
                else
                {
                    pkt.state = MENU;
                }
                break;
            case REGISTER:
                printf("\nEnter User Name: ");
                scanf(" %31[^\n]", pkt.user.username);
                printf("Enter Password: ");
                scanf(" %31[^\n]", pkt.user.password);
                ret = register_user(&pkt, sockfd);
                if(ret == SUCCESS)
                {
                    pkt.state = CHAT;
                }
                else
                {
                    pkt.state = MENU;
                }
                break;
            case CHAT:
                handle_chatopt(&pkt, sockfd);
                break;
            case SINGLE_CHAT:
                printf("\nConnected to: %s\n", pkt.chat.receiver);
                // ctrl+C to leave chat
                // register a signal for handling SIGINT (ctrl + C)
                // after existing from chat it goes to chat menu option where user can logout
                signal(SIGINT, signal_handler);
                single_chat(&pkt, sockfd); 
                break;
            case MULTI_CHAT:
                printf("\nConnected to Group Chat\n");
                // ctrl+C to leave chat
                signal(SIGINT, signal_handler);
                multi_chat(&pkt, sockfd);
                break;
            case LOGOUT:
                pkt.state = LOGOUT;
                // notify server about axiting
                if(send(sockfd, &pkt, sizeof(packet_t), 0) == -1)
                {
                    perror("send");
                    continue;
                }
                printf("\nExiting....\n");
                close(sockfd);
                exit(0);
                break;
        }
        
    }

    return SUCCESS;
}