# TCP/IP Chat Room
This project implements a TCP-based Chat Room application using the client-server model in C. The server manages multiple client connections concurrently. The clients can register, log in, participate in private and group chat. User details like the username and password is stored in a text file, and communication is established using packet structures over TCP sockets.

## Features
- TCP client-server concurrent architecture
- User registration and log in
- Single user (priate) chat
- Multi user (group) chat
- User logout
- Notifications
- Menu-driven interface
- Signal handling for SIGINT

## Working
The server listens for incoming client connections and manages concurrency using select(). Users can opt for register and log in using username and password which are then stored in a text file.

After authetication, users can start a private chat or join a group chat. Messages are forwarded by the server to respective receivers who are currently logged in. All the online user information are also stored in a structure array for comparisons.

## Technologies Used
- C Programming
- TCP Socket Programming
- Client-Server Architecture
- Concurrent client handling
- Signal Handling
- File Handling

## Project Structure
```bash
.
├── client
│   ├── makefile
│   ├── tcp_c.c
│   ├── tcp_c.h
│   └── tcp_client.c
└── server
    ├── makefile
    ├── tcp_s.c
    ├── tcp_s.h
    ├── tcp_server.c
    └── users.txt
```

## Build Instructions
Compile the server and client using:
```bash
make
```

## Usage
Start the server using:
```bash
./server
```

Start the client using:
```bash
./client
```

The client provides the following menu:
1. Login
2. Register
3. Exit

After successful login/register:
Chat Options:
1. Single User Chat
2. Multi User Chat
3. Logout

- For leaving from a private/group chat use signal SIGNIT (ctrl + c)
- The client will be then back to the Chat options menu
- To leave the Chat Room/ to exit opt for Logout option in the Chat options menu for graceful termination
