/* Name         : Naura Yasmeen U
   Reg No.      : 25017_094
   Start Date   : 25/05/2026
   End Date     : 29/05/2026
   Project Name : Trivial File Transfer Protocol
*/
#include "tftp.h"
#include "tftp_client.h"

int main() {
    tftp_client_t client;
    memset(&client, 0, sizeof(client));  // Initialize client structure

    // initially set mode as normal mode
    strcpy(client.mode, "normal"); 

    // Main loop for client-server interaction
    while (1) {
        int op;
        char ip[30], file_name[256];
        printf("\n----Client Menu----\n1. Connect to Server\n2. Put file\n3. Get file\n4. Set mode\n5. Exit\nSelect one option : ");
        scanf("%d", &op);
        switch(op)
        {
            case 1:
                printf("Enter the ip: ");
                scanf(" %[^\n]", ip);
                connect_to_server(&client, ip, PORT);
                break;

            case 2:
                printf("Enter the file name to put : ");
                scanf(" %[^\n]", file_name);
                put_file(&client, file_name);
                break;

            case 3:
                printf("Enter the file name to get : ");
                scanf(" %[^\n]", file_name);
                get_file(&client, file_name);
                break;

            case 4:
                int choice;
                printf("\n1. Normal Mode\n");
                printf("2. Octet Mode\n");
                printf("3. Netascii mode\n");
                printf("Enter choice: ");
                scanf("%d", &choice);
                switch(choice)
                {
                    case 1:
                        strcpy(client.mode, "normal");
                        printf("Mode set to NORMAL\n");
                        break;
                    case 2:
                        strcpy(client.mode, "octet");
                        printf("Mode set to Octet\n");
                        break;
                    case 3:
                        strcpy(client.mode, "netascii");
                        printf("Mode set to NETASCII\n");
                        break;
                    default:
                        printf("Invalid mode\n");
                }
                break;

            case 5:
                disconnect(&client);
                exit(0);

            default:
                printf("Invalid choice\n");
        }
        fflush(stdin);
    }

    return 0;
}

void connect_to_server(tftp_client_t *client, char *ip, int port) 
{
    if(client->connected)
    {
        printf("Already connected\n");
        return;
    }

    int i = 0, dot_count = 0;
    // validate ip address
    if(!(isdigit(ip[0])))
    {
        printf("Error : ip address is invalid\n");
        return;
    }
    while(ip[i] != '\0')
    {
        if(ip[i] == '.')
        {
            dot_count++;
            if(ip[i + 1] == '\0' || !(isdigit(ip[i + 1])))
            {
                printf("Error : ip address is invalid\n");
                return;
            }
        }
        else if(!(isdigit(ip[i])))
        {
            printf("Error : ip address is invalid\n");
            return;
        }
        i++;
    }
    if(dot_count != 3)
    {
        printf("Error : ip address is invalid\n");
        return;
    }

    // Create UDP IPv4 socket
    client->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client->sockfd < 0)
    {
        perror("Socket creation failed");
        return;
    }

    // save server address
    memset(&client->server_addr, 0, sizeof(client->server_addr));
    client->server_addr.sin_family = AF_INET;
    client->server_addr.sin_port = htons(PORT);
    client->server_addr.sin_addr.s_addr = inet_addr(ip);
    client->server_len = sizeof(client->server_addr);

    client->connected = 1;
    printf("Connected to server\n");
}

void put_file(tftp_client_t *client, char *filename)
{
    if (client->connected == 0)
    {
        printf("ERROR: Connect to the server first\n");
        return;
    }
    int recv_len;

    // validate file
    int file_fd = open(filename, O_RDONLY);
    if(file_fd == -1)
    {
        printf("Error: File not found\n");
        return;
    }

    // Send file with write request
    send_request(client, filename, WRQ);

    // recieve ACK from server
    tftp_packet packet;
    recv_len = recvfrom(client->sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&client->server_addr, &client->server_len);
    if (recv_len < 0)
    {
        perror("recvfrom");
        return;
    }
    if(ntohs(packet.opcode) != ACK || ntohs(packet.body.ack_packet.block_number) != 0)
    {
        printf("ERROR: Invalid ACK packet\n");
        close(file_fd);
        return;
    }
    send_file(client, file_fd); 
}

void get_file(tftp_client_t *client, char *filename) 
{
    if (client->connected == 0)
    {
        printf("ERROR: Connect to the server first\n");
        return;
    }

    int recv_len;
    // send file name with read request 
    send_request(client, filename, RRQ);

    tftp_packet packet;

    // recieve ACK from server
    recv_len = recvfrom(client->sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&client->server_addr, &client->server_len);
    if (recv_len < 0)
    {
        perror("recvfrom");
        return;
    }
    if(ntohs(packet.opcode) != ACK || ntohs(packet.body.ack_packet.block_number) != 0)
    {
        printf("ERROR: Invalid ACK packet\n");
        return;
    }

    // check if file already exists
    int file_fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if(file_fd == -1)
    {
        if(errno == EEXIST)
        {
            file_fd = open(filename, O_TRUNC | O_WRONLY, 0644);
        }
        else
        {
            perror("open");
            return;
        }
    }
    receive_file(client, file_fd);
}

void disconnect(tftp_client_t *client)
{
    // close fd
    close(client->sockfd);
    client->connected = 0;
    printf("Disconnected from server\n");
}

void send_request(tftp_client_t *client, char *filename, int opcode)
{
    tftp_packet packet;
    memset(&packet, 0, sizeof(packet));
    packet.opcode = htons(opcode);
    strcpy(packet.body.request.filename, filename);
    strcpy(packet.body.request.mode, client->mode);
    sendto(client->sockfd, &packet, sizeof(packet), 0, (struct sockaddr*)&client->server_addr, sizeof(client->server_addr));
}