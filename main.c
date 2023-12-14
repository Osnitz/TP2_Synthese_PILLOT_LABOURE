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

    return 0;
}
