
#include "tftp.h"
#include "tftp_client.h"

void send_file(tftp_client_t *client, int file_fd) 
{
    // Implement file sending logic 
    tftp_packet data_pkt;
    tftp_packet ack_pkt;

    int data_size;
    int recv_len;
    int chunk_size;

    uint16_t block = 1;

    while(1)
    {
        memset(&data_pkt, 0, sizeof(data_pkt));
        data_pkt.opcode = htons(DATA);
        data_pkt.body.data_packet.block_number = htons(block);

        if (strcmp(client->mode, "octet") == 0)
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
        if (strcmp(client->mode, "netascii") == 0)
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
        sendto(client->sockfd, &data_pkt, 4 + data_size, 0, (struct sockaddr *)&client->server_addr, sizeof(client->server_addr));
        
        // receive ACK
        recv_len = recvfrom(client->sockfd, &ack_pkt, sizeof(ack_pkt), 0, (struct sockaddr *)&client->server_addr, &client->server_len);
        if(recv_len < 0)
        {
            {
                perror("recvfrom");
                close(file_fd);
                return;
            }
        }

        // check ACK
        if(ntohs(ack_pkt.opcode) != ACK)
        {
            printf("ERROR: Invalid packet\n");  
            close(file_fd);
            return;
        }
        if (ntohs(ack_pkt.body.ack_packet.block_number) != block)
        {
            printf("Resending DATA packet...\n");
            // resend the data packet
            sendto(client->sockfd, &data_pkt, 4 + data_size, 0, (struct sockaddr *)&client->server_addr, sizeof(client->server_addr));
            continue;
        }

        block++;

        if(data_size < chunk_size)
        {
            break;
        }
    }
    close(file_fd);
    printf("File sent successfully\n");
}

void receive_file(tftp_client_t *client, int file_fd)
{
    // Implement file receiving logic 
    tftp_packet ack_pkt;
    tftp_packet data_pkt;

    int recv_len;
    int data_size;
    int chunk_size;

    uint16_t expected_block = 1;

    // send ACK to server
    memset(&ack_pkt, 0, sizeof(ack_pkt));
    ack_pkt.opcode = htons(ACK);
    ack_pkt.body.ack_packet.block_number = htons(0);
    sendto(client->sockfd, &ack_pkt, 4, 0, (struct sockaddr*)&client->server_addr, sizeof(client->server_addr));

    while(1)
    {
        memset(&data_pkt, 0, sizeof(data_pkt));
        // receive DATA packet
        recv_len = recvfrom(client->sockfd, &data_pkt, sizeof(data_pkt), 0, (struct sockaddr *)&client->server_addr, &client->server_len);
        if (recv_len < 0)
        {
            perror("recvfrom");
            close(file_fd);
            return;
        }
        
        if(ntohs(data_pkt.opcode) != DATA)
        {
            printf("ERROR: Invalid packet\n");  
            close(file_fd);
            return;
        }

        // check if received block and expected blocks are same
        if (ntohs(data_pkt.body.data_packet.block_number) != expected_block)
        {
            printf("Wrong DATA block received\n");
            // send ACK for last expected block
            ack_pkt.opcode = htons(ACK);
            ack_pkt.body.ack_packet.block_number = htons(expected_block - 1);
            sendto(client->sockfd, &ack_pkt, 4, 0, (struct sockaddr *)&client->server_addr, sizeof(client->server_addr));
            continue;
        }

        data_size = recv_len - 4;

        if (strcmp(client->mode, "octet") == 0)
        {
            chunk_size = 1;
        }
        else
        {
            chunk_size = 512;
        }
        // store data in file
        if(strcmp(client->mode, "netascii") == 0)
        {
            char temp[1024];
            int j = 0;
            for(int i = 0; i < data_size; i++)
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

        // send ACK to server
        memset(&ack_pkt, 0, sizeof(ack_pkt));
        ack_pkt.opcode = htons(ACK);
        ack_pkt.body.ack_packet.block_number = htons(expected_block);
        sendto(client->sockfd, &ack_pkt, 4, 0, (struct sockaddr*)&client->server_addr, sizeof(client->server_addr));

        if (data_size < chunk_size)
        {
            break;
        }
        expected_block++;
    }
    close(file_fd);
    printf("File received successfully\n");
}