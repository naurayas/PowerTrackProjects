# Trivial File Transfer Protocol (TFTP)
This project implements a simplified Trivial File Transfer Protocol (TFTP) using UDP sockets in C. It consists of seperate client and server applications that supports uploading and downloading files through write and read requests. The implementation follows basic TFTP protocol by exchanging data through DATA and ACK packets to ensure reliable file transfer over UDP.

## Features
- Client-Server architecture using UDP sockets
- Supports Read Request (RRQ) and Write Request(WRQ)
- Supports file upload (put) and download (get)
- Block wise file transfer using packets
- Supports file transfer in normal, octet and netascii modes
- Ensures reliabilty through block number validation

## Working
Initially the client connects to the server using UDP sockets. The client can send a read request (RRQ) to download a file from the server or send a write request (WRQ) to upload a file to the sever along with the file name and the transfer mode (normal - default, octet or netascii).

The server processes the request and transfers the files as DATA packets. The receiver acknowledges each block with the ACK packts.

The transfer completes when the final DATA packet contains less than the maximum block size.

## Technologies Used
- C Programming
- UDP Socket Programming
- Linux System Calls
- File Handling
- TFTP Protocol
- Structure and Union

## Project Structure
```bash
.
├── client
│   ├── file.txt
│   ├── makefile
│   ├── tftp.c
│   ├── tftp.h
│   ├── tftp_client.c
│   └── tftp_client.h
└── server
    ├── file.txt
    ├── makefile
    ├── tftp.c
    ├── tftp.h
    └── tftp_server.c
```

## Build Instructions
Compile the server and client using:
```bash
make
```

## Usage
Start the server:
```bash
./server
```
Start the client:
```bash
./client
```

The client provides the following menu:
1. Connect to Server
2. Put file
3. Get file
4. Set mode
5. Exit

<img width="980" height="742" alt="client_out" src="https://github.com/user-attachments/assets/a62cdaf4-8843-42f7-a8c5-d2d094cb37b1" />
<img width="937" height="215" alt="server_out" src="https://github.com/user-attachments/assets/7d5449fb-63ed-403c-9ce2-a312c7fe46db" />

