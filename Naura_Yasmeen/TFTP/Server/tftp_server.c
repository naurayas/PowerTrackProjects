/* Name         : Naura Yasmeen U
   Reg No.      : 25017_094
   Start Date   : 25/05/2026
   End Date     : 29/05/2026
   Project Name : Trivial File Transfer Protocol
*/
#include "tftp.h"

int main() 
{
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    tftp_packet packet;

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        perror("Socket creation failed");
        return 0;
    }
    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("TFTP Server listening on port %d...\n", PORT);

    // handle incoming requests
    while (1) {
        int recv_len = recvfrom(sockfd, &packet, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        if (recv_len < 0) 
        {
            perror("recvfrom");
            continue;
        }
        handle_client_request(sockfd, client_addr, client_len, &packet);
    }

    close(sockfd);
    return 0;
}

void handle_client_request(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, tftp_packet *packet) 
{
    // Extract the TFTP request (read or write) from the received packet
    if(ntohs(packet->opcode) == WRQ)
    {
        printf("WRQ received for file: %s\n", packet->body.request.filename);
        receive_file(sockfd, client_addr, client_len, packet->body.request.filename, packet->body.request.mode);
    }
    else if(ntohs(packet->opcode) == RRQ)
    {
        printf("RRQ received for file: %s\n", packet->body.request.filename);
        send_file(sockfd, client_addr, client_len, packet->body.request.filename, packet->body.request.mode);
    }
    else
    {
        printf("Error: Invalid request\n");
    }
}