#include "user.h"

session user = {false, "", ""};

int main(int argc, char **argv) {
    int socket_type;
    char port[6] = DEFAULT_PORT;
    char *asip = getIpAdress();
    char *buffer = (char*)malloc(BUFFER_SIZE + 1);
    
    // Update ip and/or port 
    if (argc > 1) {
        // ip included first
        if (!strcmp(argv[1], "-n")) {
            free(asip);
            memcpy(asip, argv[2], strlen(argv[2]) + 1);
            // ip and port included
            if (argc == 5 && !strcmp(argv[3], "-p")) 
                memcpy(port, argv[4], strlen(argv[4]) + 1);

        // port included first
        } else if (!strcmp(argv[1], "-p")) { 
            memcpy(port, argv[2], strlen(argv[2]) + 1);
            // port and ip included 
            if (argc == 5 && !strcmp(argv[3], "-n")) {
                free(asip);
                memcpy(asip, argv[4], strlen(argv[4]) + 1);
            } 
        }
    }
    while (true) {
        fgets(buffer, BUFFER_SIZE, stdin);
        if (!is_input_valid(buffer, &socket_type, &user)) {
            printf("ERR: %s\n", buffer);
        } else {
            if (strcmp(buffer, "EXT\n") == 0) break;
            if (socket_type == SOCK_DGRAM) send_request_udp(port, asip, buffer);
            else if (socket_type == SOCK_STREAM) send_request_tcp(port, asip, buffer);
        }
    }
    free(buffer);
    free(asip);
}  

void send_request_udp(char *port, char *asip, char *buffer) {
    int fd, errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints, *res;
    struct sockaddr_in addr;
    
    fd = socket(AF_INET, SOCK_DGRAM, 0); 
    if (fd == -1) /*error*/exit(1);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket

    errcode = getaddrinfo(asip, port, &hints, &res);
    if (errcode != 0) /*error*/ exit(1);

    n = sendto(fd, buffer, strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
    if (n == -1) /*error*/ exit(1);

    addrlen = sizeof(addr);

    n = recvfrom(fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&addr, &addrlen);
    if (n == -1) exit(1);
    buffer[n] = '\0';

    analyze_reply_udp(buffer);
    write(1, buffer, strlen(buffer));
    freeaddrinfo(res);
    close(fd);
}

void analyze_reply_udp(char *buffer) {
    char type_reply[4];
    char status[4];
    sscanf(buffer, "%s %s", type_reply, status); 

    if (strcmp(type_reply, "RLI") == 0) { // reply for login
        if (strcmp(status, "OK") == 0) {
            user.logged = true;
            sprintf(buffer, "successful login\n");
        } else if (strcmp(status, "NOK") == 0) {
            sprintf(buffer, "incorrect login attempt\n");
        } else if (strcmp(status, "REG") == 0) {
            sprintf(buffer, "new user registered\n");
            user.logged = true;
        }
    } else if (strcmp(type_reply, "RLO") == 0) { // reply for logout
        if (strcmp(status, "OK") == 0) {
            sprintf(buffer, "successful logout\n");
            user.logged = false;
        } else if (strcmp(status, "NOK") == 0) {
            sprintf(buffer, "user not logged in\n");
        } else if (strcmp(status, "UNR") == 0) {
            sprintf(buffer, "unknown user\n");
        }
    } else if (strcmp(type_reply, "RUR") == 0) { // reply for unregister
        if (strcmp(status, "OK") == 0) {
            sprintf(buffer, "successful unregister\n");
            user.logged = false;
        } else if (strcmp(status, "NOK") == 0) {
            sprintf(buffer, "incorrect unregister attempt\n");
        } else if (strcmp(status, "UNR") == 0) {
            sprintf(buffer, "unknown user\n");
        }
    } 
    else { // cases with list
        char *list = (char *)malloc(BUFFER_SIZE);       
        if (list == NULL) exit(1);

        // Use sscanf again to skip the first two words and copy the rest to the list
        sscanf(buffer, "%*s %*s %[^\n]", list);
        if (strcmp(status, "OK") == 0 && strcmp(type_reply, "RRC") != 0) {
            char *token = strtok(list, " ");
            buffer[0] = '\0';
            char tmp[4];
            bool first = true;

            while (token != NULL) {
                strcpy(tmp, token);
                token = strtok(NULL, " ");
                if (token[0] == '1') {
                    if (!first) strcat(buffer, " ");
                    strcat(buffer, tmp);
                    first = false;
                }
                token = strtok(NULL, " ");
            }
            if (strlen(buffer) == 0) sprintf(buffer, "no auction was yet started\n");
            else strcat(buffer, "\n");
        }  
        if (strcmp(type_reply, "RMA") == 0) { // reply for myauctions
            if (strcmp(status, "NOK") == 0) {
                sprintf(buffer, "user has no ongoing auctions\n");
            } else if (strcmp(status, "NLG") == 0) {
                sprintf(buffer, "user not logged in\n");
            }
        } else if (strcmp(type_reply, "RMB") == 0) { // reply for mybids
            if (strcmp(status, "NOK") == 0) {
                sprintf(buffer, "user has no ongoing bids\n");
            } else if (strcmp(status, "NLG") == 0) {
                sprintf(buffer, "user not logged in\n");
            }
        } else if (strcmp(type_reply, "RLS") == 0) { // reply for list
            if (strcmp(status, "NOK") == 0) {
                sprintf(buffer, "no auction was yet started\n");
            }
        } else if (strcmp(type_reply, "RRC") == 0) { // reply for show_record
            if (strcmp(status, "OK") == 0) {
                strncpy(buffer, list, sizeof(list) - 1);
            } else if (strcmp(status, "NOK") == 0) {
                sprintf(buffer, "the auction does not exist\n");
            } 
        }
        free(list);
    }
}

void send_request_tcp(char *port, char *asip, char *buffer) {
    int fd, errcode;
    ssize_t n;
    struct addrinfo hints, *res;
    fd=socket(AF_INET,SOCK_STREAM,0); // TCP socket
    if (fd==-1) exit(1); //error

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //TCP socket
    
    printf("%s", buffer);

    errcode=getaddrinfo(asip,port,&hints,&res);
    if(errcode!=0)/*error*/exit(1);
    puts("OLA");
    n=connect(fd,res->ai_addr,res->ai_addrlen);
    if(n==-1)/*error*/exit(1);

    n=write(fd, buffer, strlen(buffer));
    if(n==-1)/*error*/exit(1);

    n=read(fd,buffer,128);
    if(n==-1)/*error*/exit(1);
    buffer[n] = '\0';
    
    write(1,buffer,n);

    freeaddrinfo(res);
    close(fd);
}

void analyze_reply_tcp(int fd, char *buffer) {
    char type_reply[4];
    char status[4];
    sscanf(buffer, "%s %s", type_reply, status);
    puts(buffer);    
    return;
}
