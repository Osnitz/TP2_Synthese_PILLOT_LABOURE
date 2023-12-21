#include "function.h"

void receiveACK(int sockfd, short expectedBlockNumber) {
    char ackPacket[4];
    struct sockaddr peer_addr;
    int size_peer = sizeof (peer_addr);
    // Wait to receive an ACK packet from the server
    ssize_t bytesReceived = recvfrom(sockfd, ackPacket, sizeof(ackPacket), 0,
                                     &peer_addr, &size_peer);
    if (bytesReceived == -1) {
        error("Error receiving ACK");
    }

    // Extract the opcode
    short opcode = ntohs(*(short *)ackPacket);

    // Check if it's an ACK packet
    if (opcode != OPCODE_ACK) {
        error("Unexpected packet received, not an ACK");
    }

    // Extract the block number
    short receivedBlockNumber = ntohs(*(short *)(ackPacket + 2));

    // Check if the received block number matches the expected block number
    if (receivedBlockNumber != expectedBlockNumber) {
        error("Unexpected received block number");
    }
}

void sendWRQ(int sockfd, const char *filename, struct addrinfo *server_info) {
    // Construct a Write Request (WRQ) packet
    char requestPacket[MAX_BUFFER_SIZE];

    // Opcode for a Write Request (WRQ)
    short opcode = htons(OPCODE_WRQ); // OPCODE_WRQ is a constant defined according to the TFTP protocol, 2 for WRITE

    // Copy the opcode into the packet
    memcpy(requestPacket, &opcode, sizeof(opcode));

    // Copy the filename into the packet
    strcpy(requestPacket + 2, filename);

    // Add the mode.
    strcpy(requestPacket + 2 + strlen(filename) + 1, mode);

    // Send the WRQ to the server
    if (sendto(sockfd, requestPacket, 2 + strlen(filename) + 1 + strlen(mode) + 1, 0,
               server_info->ai_addr, server_info->ai_addrlen) == -1) {
        error("Error sending TFTP WRQ");
    }
}

void sendFileData(int sockfd, const char *filename, struct addrinfo *server_info) {
    // Open the file for reading
    int inputFile = open(filename, O_RDONLY);
    if (inputFile == -1) {
        error("Error opening input file");
    }

    // Read the file data
    char fileData[MAX_BUFFER_SIZE];
    ssize_t bytesRead;

    while ((bytesRead = read(inputFile, fileData, sizeof(fileData))) > 0) {
        // Construct a Data packet
        char dataPacket[MAX_BUFFER_SIZE];

        // Opcode for a Data packet
        short opcode = htons(OPCODE_DATA); // OPCODE_DAT is a constant defined according to the TFTP protocol, 3 for DATA

        printf("aant &ER ");
        // Copy the opcode into the packet
        memcpy(dataPacket, &opcode, sizeof(opcode));
        printf("apres 1er");

        // Copy the block number into the packet
        short blockNumber = htons(1); // For simplicity starting with 1 block
        memcpy(dataPacket + 2, &blockNumber, sizeof(blockNumber));
        printf("apres 2e");
        // Copy the file data into the packet
        memcpy(dataPacket + 4, fileData, bytesRead);
        printf("apres 3");
        // Send the DAT packet to the server
        if (sendto(sockfd, dataPacket, bytesRead + 4, 0, server_info->ai_addr, server_info->ai_addrlen) == -1) {
            error("Error sending TFTP DAT");
        }
        printf("avant ACK ");
        // Wait for ACK
        receiveACK(sockfd, blockNumber);
        printf("apres recev ACK");
    }

    close(inputFile);
}


int main(int argc, char *argv[]) {
    // Check if the correct number of command-line arguments is provided
    if (argc < 4) {
        write(2, user_argument, strlen(user_argument));
        exit(1);
    }

    // Extract command-line arguments
    char *host = argv[1];
    char *port = argv[2];

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

    for(int i=3;i<argc;i++){
        printf("d");
        sendWRQ(sockfd, argv[i], server_info);
        printf("apres send");
        sendFileData(sockfd, argv[i], server_info);
        printf("apres send dataa");

    }

    // Close the socket when done
    close(sockfd);

    return 0;
}
