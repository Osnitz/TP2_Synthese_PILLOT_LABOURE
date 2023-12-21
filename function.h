#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>


#define MAX_BUFFER_SIZE 516  // Maximum size of a TFTP packet
#define user_argument "Use: ./program <host> <port> <file>\n"
#define OPCODE_RRQ 1 // OPCODE_RRQ is a constant defined according to the TFTP protocol, 1 for READ
#define OPCODE_WRQ 2 // OPCODE_WRQ is a constant defined according to the TFTP protocol, 2 for Write
#define OPCODE_DATA 3 // OPcode for data block
#define OPCODE_ACK 4 // OPCODE for ACK in TFTP

#define mode "octet" //The mode field contains the string "netascii", "octet", or "mail"

void error(char *msg);
void sendACK(int sockfd, short blockNumber, struct addrinfo *server_info);