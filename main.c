#include "function.h"

void error(char *msg) {
    perror(msg);
    exit(1);
}

void sendTFTPRequest(int sockfd, const char *filename, struct addrinfo *server_info) {
    // Constructing a Read Request (RRQ) packet
    char requestPacket[MAX_BUFFER_SIZE];

    // Opcode for a Read Request (RRQ)
    short opcode = htons(OPCODE_RRQ); // OPCODE_RRQ is a constant defined according to the TFTP protocol, 1 for READ

    // Copying the opcode into the packet
    memcpy(requestPacket, &opcode, sizeof(opcode));

    // Copying the filename into the packet
    strcpy(requestPacket + 2, filename);

    // Adding the mode.
    strcpy(requestPacket + 2 + strlen(filename) + 1, mode);

    // Sending the request to the server
    if (sendto(sockfd, requestPacket, 2 + strlen(filename) + 1 + strlen(mode) + 1, 0,
               server_info->ai_addr, server_info->ai_addrlen) == -1) {
        error("Error sending TFTP request");
    }
}

void sendACK(int sockfd, short blockNumber, struct addrinfo *server_info) {
    char ackPacket[4];
    short opcodeACK = htons(OPCODE_ACK);
    memcpy(ackPacket, &opcodeACK, sizeof(opcodeACK));

    blockNumber = htons(blockNumber);  // Convert block number to network byte order
    memcpy(ackPacket + 2, &blockNumber, sizeof(blockNumber));

    // Send the ACK packet to the server
    if (sendto(sockfd, ackPacket, sizeof(ackPacket), 0, server_info->ai_addr, server_info->ai_addrlen) == -1) {
        error("Error sending ACK");
    }
}

void receiveTFTPData(int sockfd, const char *outputFileName, struct addrinfo *server_info) {
    char dataPacket[MAX_BUFFER_SIZE];

    // Receive the data packet from the server
    ssize_t bytesRead = recvfrom(sockfd, dataPacket, sizeof(dataPacket), 0, NULL, NULL);
    if (bytesRead == -1) {
        error("Error receiving TFTP data");
    }

    // Extract opcode
    short opcode = ntohs(*(short *)dataPacket);

    // Check if it's a Data packet (opcode 3)
    if (opcode != 3) {
        error("It's not a Data Packet");
        return;
    }

    // Extract block number
    short blockNumber = ntohs(*(short *)(dataPacket + 2));

    // Access the file data (the data starts at index 4)
    const char *fileData = dataPacket + 4;

    // Write the data to a file
    int outputFile = open(outputFileName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (outputFile == -1) {
        error("Error opening output file");
    }

    // Write the actual data to the file
    if (write(outputFile, fileData, bytesRead - 4) == -1) {
        error("Error writing data to file");
    }
    close(outputFile);
    sendACK(sockfd,blockNumber,server_info);
}

int main(int argc, char *argv[]) {
    // Check if the correct number of command-line arguments is provided
    if (argc != 4) {
        write(2, user_argument, strlen(user_argument));
        exit(1);
    }

    // Extract command-line arguments
    char *host = argv[1];
    char *port = argv[2];
    char *file = argv[3];

    // Set up address information for the server
    struct addrinfo hints, *server_info;
    memset(&hints, 0, sizeof(hints));

    // (code for setting up hints structure)
    hints.ai_family = AF_INET;//AF_INET (ipv4)
    hints.ai_socktype = SOCK_DGRAM;  // sockets UDP for TFTP
    hints.ai_protocol=IPPROTO_UDP;

    // Get address information
    if (getaddrinfo(host, port, &hints, &server_info) != 0) {
        error("Error obtaining server address");
    }

    // Create a socket
    int sockfd = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (sockfd == -1) {
        error("Error creating socket");
    }

    // Free the address information
    freeaddrinfo(server_info);

    // gettftp
    sendTFTPRequest(sockfd,file,server_info);
    receiveTFTPData(sockfd,"data_Received",server_info);
    // Close the socket when done
    close(sockfd);

    return 0;
}
