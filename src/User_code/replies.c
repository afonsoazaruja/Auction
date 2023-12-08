#include "replies.h"
#include "commands.h"

void analyze_reply_udp(char *buffer) {
    char type_reply[TYPE_REPLY_SIZE + 1];
    char status[MAX_STATUS_SIZE + 1];
    if (sscanf(buffer, "%3s %3s", type_reply, status) != 2) exit(1); 

    if (!validate_buffer(buffer)) {
        sprintf(buffer, "invalid reply from server\n");
    } else if (strcmp(status, "ERR") == 0) {
        sprintf(buffer, "invalid syntax or values\n");
    } else if (strcmp(type_reply, "RLI") == 0) {
        reply_login(status, buffer);
    } else if (strcmp(type_reply, "RLO") == 0) {
        reply_logout(status, buffer); 
    } else if (strcmp(type_reply, "RUR") == 0) {
        reply_unregister(status, buffer);         
    } else 
    { // cases with list                   
        char *list = (char *)malloc(BUFFER_SIZE);       
        if (list == NULL) exit(1);
        sscanf(buffer, "%*s %*s %[^\n]", list);

        if (strcmp(status, "OK") == 0 && strcmp(type_reply, "RRC") != 0) {
            handle_auctions(list, buffer, type_reply);
        }  
        else if (strcmp(type_reply, "RMA") == 0) { 
            reply_myauctions(status, buffer);
        } else if (strcmp(type_reply, "RMB") == 0) { 
            reply_mybids(status, buffer);
        } else if (strcmp(type_reply, "RLS") == 0) { 
            reply_list(status, buffer);
        } else if (strcmp(type_reply, "RRC") == 0) { 
            reply_show_record(status, buffer, list);
        }
        free(list);
    }
}

void handle_auctions(char *list, char *buffer, char *type) {
    char *token = strtok(list, " ");
    char tmp[4];
    buffer[0] = '\0';

    if (strcmp(type, "RLS") == 0) {
        strcat(buffer, "ACTIVE AUCTIONS:\n");
        while (token != NULL) {
            strcpy(tmp, token);
            token = strtok(NULL, " ");
            if (token[0] == '1') {
                strcat(buffer, "AID: ");
                strcat(buffer, tmp);
                strcat(buffer, "\n");
            }
            token = strtok(NULL, " ");
        }
        if (strlen(buffer) == 0) sprintf(buffer, "no auction was yet started\n");
    }    
    else if ((strcmp(type, "RMA") == 0) || (strcmp(type, "RMB") == 0)) {
        while (token != NULL) {
            strcpy(tmp, token);
            token = strtok(NULL, " ");
            strcat(buffer, "AID: ");
            strcat(buffer, tmp);
            if (token[0] == '1')
                strcat(buffer, " ACTIVE\n");
            else 
                strcat(buffer, " INACTIVE\n");
            token = strtok(NULL, " ");
        }
    }
}

void reply_login(char *status, char *buffer) {
    if (strcmp(status, "OK") == 0) {
        sprintf(buffer, "successful login\n");
        user.logged = true;
    } else if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "incorrect login attempt\n");
    } else if (strcmp(status, "REG") == 0) {
        sprintf(buffer, "new user registered\n");
        user.logged = true;
    } 
}

void reply_logout(char *status, char *buffer) {
    if (strcmp(status, "OK") == 0) {
        sprintf(buffer, "successful logout\n");
        user.logged = false;
    } else if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "user not logged in\n");
    } else if (strcmp(status, "UNR") == 0) {
        sprintf(buffer, "unknown user\n");
    } 
}

void reply_unregister(char *status, char *buffer) {
    if (strcmp(status, "OK") == 0) {
        sprintf(buffer, "successful unregister\n");
        user.logged = false;
    } else if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "incorrect unregister attempt\n");
    } else if (strcmp(status, "UNR") == 0) {
        sprintf(buffer, "unknown user\n");
    } 
}

void reply_myauctions(char *status, char *buffer) {
    if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "user has no ongoing auctions\n");
    } else if (strcmp(status, "NLG") == 0) {
        sprintf(buffer, "user not logged in\n");
    }
}

void reply_mybids(char *status, char *buffer) {
    if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "user has no ongoing bids\n");
    } else if (strcmp(status, "NLG") == 0) {
        sprintf(buffer, "user not logged in\n");
    } 
}

void reply_list(char *status, char *buffer) {
    if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "no auction was yet started\n");
    } 
}

void reply_show_record(char *status, char *buffer, const char *list) {
    if (strcmp(status, "OK") == 0) {
        strncpy(buffer, list, strlen(list));
    } else if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "the auction does not exist\n");
    } 
}

void analyze_reply_tcp(char *buffer, int fd) {
    char type_reply[4] = "";
    char status[4] = "";
    extract(buffer, type_reply, fd);    
    extract(buffer, status, fd);         

    if (strcmp(type_reply, "ROA") == 0) { 
       reply_open(status, buffer, fd);
    } else if (strcmp(type_reply, "RCL") == 0) { 
        reply_close(status, buffer);
    } else if (strcmp(type_reply, "RSA") == 0) { 
        reply_show_asset(status, buffer, fd);
    } else if (strcmp(type_reply, "RBD") == 0) { 
        reply_bid(status, buffer);
    } 
}   

void reply_open(char *status, char *buffer, int fd) {
    char aid[SIZE_AID+1] = "";
    if (strcmp(status, "OK") == 0) {
        extract(buffer, aid, fd); // AID
        // sscanf(buffer, "%*s %*s %s", aid);
        sprintf(buffer, "auction successfully created AID %s\n", aid);
    } else if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "auction could not be started\n");
    } else if (strcmp(status, "NLG") == 0) {
        sprintf(buffer, "user not logged in\n");
    } else if (strcmp(status, "EAU") == 0) {
        sprintf(buffer, "auction AID does not exist\n");
    } else if (strcmp(status, "EOW") == 0) {
        sprintf(buffer, "auction is not owned by user UID\n");
    } else if (strcmp(status, "END") == 0) {
        sprintf(buffer, "auction time already ended\n");
    } 
}

void reply_close(char *status, char *buffer) {
    if (strcmp(status, "OK") == 0) {
        sprintf(buffer, "auction successfully closed\n");
    } else if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "user does not exist or incorrect password\n");
    } else if (strcmp(status, "NLG") == 0) {
        sprintf(buffer, "user not logged in\n");
    } else if (strcmp(status, "EAU") == 0) {
        sprintf(buffer, "auction AID does not exist\n");
    } else if (strcmp(status, "EOW") == 0) {
        sprintf(buffer, "auction is not owned by user UID\n");
    } else if (strcmp(status, "END") == 0) {
        sprintf(buffer, "auction time already ended\n");
    } 
}

void reply_show_asset(char *status, char *buffer, int fd) {
    if (strcmp(status, "OK") == 0) {
        ssize_t n = 0;
        char fname[12 + MAX_FILENAME + 1] = "";
        char *endptr = "";
        char fsize[8 + 1] = ""; // 10*10⁶ (8 digitos)
        long size = 0;

        memset(buffer, 0, BUFFER_SIZE);
        extract(buffer, fname, fd);
        extract(buffer, fsize, fd);
        
        size = strtol(fsize, &endptr, 10);
        char *data = malloc(size);
        FILE *file = fopen(fname, "wb");
        if (file == NULL) {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }
        do { // read bytes of file and write
            n=read(fd, data, size);
            if(n==-1)/*error*/exit(1);
            size_t bytes_written = fwrite(data, 1, n, file);
            if (bytes_written != n) {
                perror("Error writing to file");
                fclose(file);
                exit(EXIT_FAILURE);
            }
            size -= n;
        } while(size != 0);

        sprintf(buffer, "asset transfer: success\n");
        fclose(file);
        free(data);
    } else if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "asset transfer: failure\n");
    } 
}

void reply_bid(char *status, char *buffer) {
    if (strcmp(status, "ACC") == 0) {
        sprintf(buffer, "bid accepted\n");
    } else if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "auction AID is not active\n");
    } else if (strcmp(status, "NLG") == 0) {
        sprintf(buffer, "user not logged in\n");
    } else if (strcmp(status, "REF") == 0) {
        sprintf(buffer, "a bigger bid was already made.\n");
    } else if (strcmp(status, "ILG") == 0) {
        sprintf(buffer, "you cannot make a bid in an auction hosted by yourself\n");
    } 
}

void extract(char *src, char *dst, int fd) {
    ssize_t n = 0;
    memset(src, 0, BUFFER_SIZE);
    while(true) {
        n=read(fd, src, 1);
        if(n==-1)/*error*/exit(EXIT_FAILURE);
        if (src[0] == ' ' || src[0] == '\n') break;
        src[n] = '\0';
        strcat(dst, src);
    }
}
