#include "as.h"

#define DEFAULT_PORT "58023"

int num_aid = 0;
bool verbose = false;

int main(int argc, char **argv) {
    char port[6] = DEFAULT_PORT;

    // Update port and/or set verbose mode
    if (argc > 1) {
        if (!strcmp(argv[1], "-p")) {
            memcpy(port, argv[2], strlen(argv[2]) + 1);
            if (argc == 4 && !strcmp(argv[3], "-v")) verbose = true;
        } else if (!strcmp(argv[1], "-v")) verbose = true;  
    }
    if (verbose) printf("> port: %s\n> verbose: %d\n", port, verbose);

    handle_requests(port);
}
