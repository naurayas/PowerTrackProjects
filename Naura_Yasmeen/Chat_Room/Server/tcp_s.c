#include "tcp_s.h"

void handle_register(packet_t *pkt, client_t *client)
{
    // open file (users.txt)
    int filefd = open("users.txt", O_RDWR | O_CREAT, 0644);
    if(filefd == -1)
    {
        perror("open");
        pkt->status = FAILURE;
        return;
    }

    // read the file
    char buffer[4096];
    int bytes = read(filefd, buffer, sizeof(buffer) - 1);
    if(bytes == -1)
    {
        perror("read");
        pkt->status = FAILURE;
        return;
    }
    buffer[bytes] = '\0';

    // tokenize the file data and collect each lines
    char *line = strtok(buffer, "\n");
    while (line)
    {
        // check if user name found in the users register
        char existing_user[32];
        sscanf(line, "%31[^:]", existing_user);
        if(strcmp(existing_user, pkt->user.username) == 0)
        {
            pkt->status = DUPLICATE_USER;
            close(filefd);
            return;
        }
        line = strtok(NULL, "\n");
    }

    // register as new user
    lseek(filefd, 0, SEEK_END);
    char new_user[80];
    snprintf(new_user, sizeof(new_user), "%s:%s\n", pkt->user.username, pkt->user.password);
    // write to file
    if(write(filefd, new_user, strlen(new_user)) == -1)
    {
        perror("write");
        pkt->status = FAILURE;
        close(filefd);
        return;
    }

    // user registered sucessful
    pkt->status = SUCCESS;
    strcpy(client->username, pkt->user.username);
    client->logged_in = 1;
    close(filefd);
}

void handle_login(packet_t *pkt, client_t *client)
{
    // open file (users.txt)
    int filefd = open("users.txt", O_RDONLY);
    if(filefd == -1)
    {
        perror("open");
        pkt->status = FAILURE;
        return;
    }

    // read the file
    char buffer[4096];
    int bytes = read(filefd, buffer, sizeof(buffer) - 1);
    if(bytes == -1)
    {
        perror("read");
        pkt->status = FAILURE;
        return;
    }
    buffer[bytes] = '\0';

    // tokenize the file data and collect each lines
    char *line = strtok(buffer, "\n");
    int size;

    char existing_user[32];
    char existing_pass[32];

    while (line)
    {
        // check if user name found in the users register
        sscanf(line, "%31[^:]:%31[^\n]", existing_user, existing_pass);
        if(strcmp(existing_user, pkt->user.username) == 0)
        {
            // compare password
            if(strcmp(existing_pass, pkt->user.password) == 0)
            {
                pkt->status = SUCCESS;
                // update online client
                strcpy(client->username, pkt->user.username);
                client->logged_in = 1;
            }
            else
            {
                pkt->status = PASSWORD_MISMATCH;
            }
            close(filefd);
            return;
        }
        line = strtok(NULL, "\n");
    }

    // user not found
    pkt->status = USER_NOT_FOUND;
    close(filefd);
}

void handle_online_rqt(packet_t *pkt, client_t *clients)
{
    int user_count = 0;
    // search the client list for logged in users
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        // copy loggen in users into the packet
        if (clients[i].logged_in == 1)
        {
            if(!strcmp(pkt->user.username, clients[i].username))
                continue;
            strcpy(pkt->online.users[user_count++], clients[i].username);
        }
    }
    pkt->online.count = user_count;
}

void handle_single_chat(packet_t *pkt, client_t *clients)
{
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        if(!strcmp(pkt->chat.receiver, clients[i].username))
        {
            //printf("\nReceiver found\n");
            //printf("Username: %s\nFD: %d\n", clients[i].username, clients[i].fd);
            // send the same packet containing message to receiver
            if(send(clients[i].fd, pkt, sizeof(packet_t), 0) == -1)
            {
                perror("send");
                return;
            }
        }
    }
}

void handle_multi_chat(packet_t *pkt, client_t *clients, int senderfd)
{
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        // send message to all logged in clients except the sender
        if(clients[i].logged_in && clients[i].fd != senderfd)
        {
            if(send(clients[i].fd, pkt, sizeof(packet_t), 0) == -1)
            {
                perror("send");
                return;
            }
        }
    }
}

void handle_logout(packet_t *pkt, client_t *clients, int clientfd, fd_set *masterfds)
{
    sprintf(pkt->chat.message, "%s is offline\n", pkt->user.username);
    // send offline message to all logged in clients
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        // clear the exited client
        if(clients[i].fd == clientfd)
        {
            // remove from fdset
            FD_CLR(i, masterfds);
            close(i);
            clients[i].fd = -1;
            clients[i].logged_in = 0;
            memset(clients[i].username, 0, sizeof(clients[i].username));
        }
        // send message to all logged in clients except the sender
        if(clients[i].logged_in)
        {
            if(send(clients[i].fd, pkt, sizeof(packet_t), 0) == -1)
            {
                perror("send");
                return;
            }
        }
    }
}