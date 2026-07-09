/* Common file for server & client */

#include "tftp.h"

void send_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename, char *mode) 
{
    // Implement file sending logic here
    tftp_packet error_pkt, ack_pkt, data_pkt;
    int recv_len, data_size;
    uint16_t block = 1;
    int chunk_size;

    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0)
    {
        perror("open");
        return;
    }

    // send ACK
    memset(&ack_pkt, 0, sizeof(ack_pkt));
    ack_pkt.opcode = htons(ACK);
    ack_pkt.body.ack_packet.block_number = htons(0);
    sendto(sockfd, &ack_pkt, 4, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));

    // receive ACK from client (after client varifies its file existence)
    recv_len = recvfrom(sockfd, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&client_addr, &client_len);
    if (recv_len < 0)
    {
        perror("recvfrom");
        close(file_fd);
        return;
    }

    if (ntohs(ack_pkt.opcode) != ACK || ntohs(ack_pkt.body.ack_packet.block_number) != 0)
    {
        printf("ERROR: Invalid ACK packet\n");
        close(file_fd);
        return;
    }

    while(1)
    {
        // send DATA packets
        memset(&data_pkt, 0, sizeof(data_pkt));
        data_pkt.opcode = htons(DATA);
        data_pkt.body.data_packet.block_number = htons(block);
        
        // read from file
        if (strcmp(mode, "octet") == 0)
        {
            data_size = read(file_fd, data_pkt.body.data_packet.data, 1);
            chunk_size = 1;
        }
        else
        {
            data_size = read(file_fd, data_pkt.body.data_packet.data, 512);
            chunk_size = 512;
        }
        if (data_size < 0)
        {
            perror("read");
            close(file_fd);
            return;
        }

        // send DATA packet
        if (strcmp(mode, "netascii") == 0)
        {
            char temp[1024];
            int j = 0;
            for (int i = 0; i < data_size; i++)
            {
                if (data_pkt.body.data_packet.data[i] == '\n')
                {
                    temp[j++] = '\r';
                    temp[j++] = '\n';
                }
                else
                {
                    temp[j++] = data_pkt.body.data_packet.data[i];
                }
            }
            data_size = j;
            memcpy(data_pkt.body.data_packet.data, temp, j);
        }
        sendto(sockfd, &data_pkt, 4 + data_size, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));

        // receive ACK from client
        recv_len = recvfrom(sockfd, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&client_addr, &client_len);
        if (recv_len < 0)
        {
            perror("recvfrom");
            close(file_fd);
            return;
        }

        if (ntohs(ack_pkt.opcode) != ACK)
        {
            printf("Invalid ACK packet\n");
            close(file_fd);
            return;
        }
        // check ACK block 
        if(ntohs(ack_pkt.body.ack_packet.block_number) != block)
        {
            printf("Resending DATA packet...\n");
            sendto(sockfd, &data_pkt, 4 + data_size, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
            continue;
        }
        if(data_size < chunk_size)
        {
            break;
        }

        block++;
    }
    close(file_fd);
    printf("File sent successfully\n");
}

void receive_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename, char *mode) 
{
    // check if file already present
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
    int data_size;
    int recv_len;
    int chunk_size;
    uint16_t expected_block = 1;
    tftp_packet data_pkt, ack_pkt;
    
    // send ACK to client
    memset(&ack_pkt, 0, sizeof(ack_pkt));
    ack_pkt.opcode = htons(ACK);
    ack_pkt.body.ack_packet.block_number = htons(0);
    sendto(sockfd, &ack_pkt, 4, 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
    
    while(1)
    {
        // receive DATA packet from client and store it into the file
        memset(&data_pkt, 0, sizeof(data_pkt));
        recv_len = recvfrom(sockfd, &data_pkt, sizeof(data_pkt), 0, (struct sockaddr *)&client_addr, &client_len);
        if (recv_len < 0)
        {
            perror("recvfrom");
            close(file_fd);
            return;
        }
        if(ntohs(data_pkt.opcode ) != DATA)
        {
            printf("Invalid packet\n");
            close(file_fd);
            return;
        }

        // validate block number
        if (ntohs(data_pkt.body.data_packet.block_number) != expected_block)
        {
            printf("Wrong block\n");

            // resend previous ACK
            ack_pkt.opcode = htons(ACK);
            ack_pkt.body.ack_packet.block_number = htons(expected_block - 1);
            sendto(sockfd, &ack_pkt, 4, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
            continue;
        }

        // write data into file
        data_size = recv_len - 4;

        if (strcmp(mode, "octet") == 0)
        {
            chunk_size = 1;
        }
        else
        {
            chunk_size = 512;
        }
        if (strcmp(mode, "netascii") == 0)
        {
            char temp[1024];
            int j = 0;
            for (int i = 0; i < data_size; i++)
            {
                if (i + 1 < data_size && data_pkt.body.data_packet.data[i] == '\r' && data_pkt.body.data_packet.data[i + 1] == '\n')
                {
                    temp[j++] = '\n';
                    i++;
                }
                else
                {
                    temp[j++] = data_pkt.body.data_packet.data[i];
                }
            }
            write(file_fd, temp, j);
        }
        else
        {
            write(file_fd, data_pkt.body.data_packet.data, data_size);
        }

        // send ACK to client
        ack_pkt.opcode = htons(ACK);
        ack_pkt.body.ack_packet.block_number = htons(expected_block);
        sendto(sockfd, &ack_pkt, 4, 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
        
        expected_block++;

        if(data_size < chunk_size)
        {
            break;
        }
    }
    close(file_fd);
    printf("File received successfully\n");
} 