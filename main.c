#include "function.h"

void error(char *msg) {
    perror(msg);
    exit(1);
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
    struct addrinfo hints, *serverinfo;

    // (code for setting up hints structure)
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;  // sockets UDP for TFTP

    // Get address information
    if (getaddrinfo(host, port, &hints, &serverinfo) != 0) {
        error("Error obtaining server address");
    }


    return 0;
}
