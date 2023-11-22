#include "verify_commands.h"
#include <sys/types.h>


bool is_input_valid(char *buffer, char *msg) {
    char args[5][128];
    char cmd[10];
    sscanf(buffer, "%s", cmd); // extract command
    
    if (!strcmp(cmd, "login")) {
        sscanf(buffer, "%*s %s %s", args[0], args[1]); // %*s ignores 1st string
        if (!is_login_valid(args[0], args[1])) {
            sprintf(msg, "invalid UID or password\n");
            return false;
        }
        else 
            sprintf(msg, "LIN %s %s\n", args[0], args[1]);
    }
    else if (!strcmp(cmd, "logout")) {
        sprintf(msg, "LOU\n");
    }
    else if (!strcmp(cmd, "unregister")) {
        sprintf(msg, "UNR\n");
    }
    else if (!strcmp(cmd, "myauctions") || !strcmp(cmd, "ma")) {
        sprintf(msg, "LMA\n");
    }
    else if (!strcmp(cmd, "my bids") || !strcmp(cmd, "mb")) {
        sprintf(msg, "LMB\n");
    }
    else if (!strcmp(cmd, "list") || !strcmp(cmd, "l")) {
        sprintf(msg, "LST\n");
    }
    else if (!strcmp(cmd, "open")) {
        sprintf(msg, "OPA\n");
    }
    else if (!strcmp(cmd, "exit")) {
        sprintf(msg, "EXT\n");
    }
    else if (!strcmp(cmd, "show_record") || !strcmp(cmd, "sr")) {
        sprintf(msg, "SRC\n");
    }
    else {
        sprintf(msg, "Invalid input\n");
        return false;
    }
    return true;
}

bool is_login_valid(char *UID, char *password) {
    if (UID == NULL || password == NULL) return false;
    if (is_UID(UID) && is_password(password)) return true;
    return false;
}

bool is_UID(char *str) {
    int length = strlen(str);
    if (length != 6) return false;  

    for (int i = 0; i < length; i++) 
        if (!isdigit(str[i])) return false;
    return true;
}

bool is_password(char *str) {  
    int length = strlen(str);
    if (length != 8) return false;  

    for (int i = 0; i < length; i++) 
        if (!isalnum(str[i])) return false;
    return true;
}

bool verify_open(char words_command[10][20]) {
    // TO DO
    return false;
}
