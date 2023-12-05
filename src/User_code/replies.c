#include "replies.h"
#include "commands.h"

void analyze_reply_udp(char *buffer) {
    char type_reply[4];
    char status[4];
    sscanf(buffer, "%s %s", type_reply, status); 

    if (strcmp(type_reply, "RLI") == 0) {
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
            free(buffer);
            buffer = get_ongoing_auctions(list);
            if (strlen(buffer) == 0) sprintf(buffer, "no auction was yet started\n");
            else strcat(buffer, "\n");
        }  
        if (strcmp(type_reply, "RMA") == 0) { 
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

char *get_ongoing_auctions(char *list) {
    char *token = strtok(list, " ");
    char *buffer = malloc(BUFFER_SIZE);
    char tmp[4];
    bool first = true;
    buffer[0] = '\0';

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
    return buffer;
}

void reply_login(char *status, char *buffer) {
    if (strcmp(status, "OK") == 0) {
        user.logged = 1;
        sprintf(buffer, "successful login\n");
    } else if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "incorrect login attempt\n");
    } else if (strcmp(status, "REG") == 0) {
        sprintf(buffer, "new user registered\n");
        user.logged = 1;
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

void analyze_reply_tcp(char *buffer) {
    char type_reply[4];
    char status[4];
    sscanf(buffer, "%s %s", type_reply, status);

    if (strcmp(type_reply, "ROA") == 0) { 
       reply_open(status, buffer);
    } else if (strcmp(type_reply, "RCL") == 0) { 
        reply_close(status, buffer);
    } else if (strcmp(type_reply, "RSA") == 0) { 
        reply_show_asset(status, buffer);
    } else if (strcmp(type_reply, "RBD") == 0) { 
        reply_bid(status, buffer);
    }
}   

void reply_open( char *status, char *buffer) {
    // TO DO
    puts(buffer);
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

void reply_show_asset(char *status, char *buffer) {
    if (strcmp(status, "OK") == 0) {
        char fname[MAX_FILENAME + 1];
        long fsize = 0;
        sscanf(buffer, "%*s %s %ld", fname, &fsize);

        char fdata[fsize + 1];
        sscanf(buffer, "%*s %*s %*s %s", fdata);

        FILE *file = fopen(fname, "w");
        if (file == NULL) {
            perror("Error opening file");
            exit(1);
        } else {
            fprintf(file, "%s", fdata);
            fclose(file);
            sprintf(buffer, "%s %lu", fname, fsize);
        }
    } else if (strcmp(status, "NOK") == 0) {
        sprintf(buffer, "no file to be sent or other problems\n");
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
