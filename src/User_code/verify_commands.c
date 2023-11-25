#include "verify_commands.h"
#include <sys/types.h>

bool is_input_valid(char *buffer, int *socket_type, struct session *user) {
    char cmd[10];
    sscanf(buffer, "%s", cmd); // extract command
    *socket_type = SOCK_DGRAM; // assume its udp command

    // udp requests
    if (strcmp(cmd, "login") == 0) {
        char uid[128], password[128];
        if (sscanf(buffer, "%*s %s %s", uid, password) != 2 || !is_login_valid(uid, password)) {
            sprintf(buffer, "invalid UID or password");
            return false;
        } 
        else if (user->logged == true) {
            sprintf(buffer, "user already logged in");
            return false;
        }
        else {
            sprintf(buffer, "LIN %s %s\n", uid, password);
            strcpy(user->UID, uid);
            strcpy(user->password, password);
            user->logged = true;
        }
    } else if (strcmp(cmd, "logout") == 0) {
        sprintf(buffer, "LOU %s %s\n", user->UID, user->password);
        user->logged = false;
    } else if (strcmp(cmd, "unregister") == 0) {
        sprintf(buffer, "UNR %s %s\n", user->UID, user->password);
    } else if (strcmp(cmd, "myauctions") == 0 || strcmp(cmd, "ma") == 0) {
        sprintf(buffer, "LMA %s\n", user->UID);
    } else if (strcmp(cmd, "my bids") == 0 || strcmp(cmd, "mb") == 0) {
        sprintf(buffer, "LMB %s\n", user->UID);
    } else if (strcmp(cmd, "list") == 0 || strcmp(cmd, "l") == 0) {
        sprintf(buffer, "LST\n");
    } else if (strcmp(cmd, "exit") == 0) {
        sprintf(buffer, "EXT\n");
    } else if (strcmp(cmd, "show_record") == 0 || strcmp(cmd, "sr") == 0) {
        sprintf(buffer, "SRC\n");
    } 
    // tcp requests    
    else {
        if (strcmp(cmd, "open") == 0) {
            sprintf(buffer, "OPA %s %s\n", user->UID, user->password);
        } else if (strcmp(cmd, "close") == 0) {
            sprintf(buffer, "CLS %s %s\n", user->UID, user->password);
        } else if (strcmp(cmd, "show_asset") == 0) {
            sprintf(buffer, "SAS\n");
        } else if (strcmp(cmd, "bid") == 0) {
            sprintf(buffer, "BID %s %s\n", user->UID, user->password);
        } else {
            sprintf(buffer, "Invalid input");
            return false;
        }
        *socket_type = SOCK_STREAM;
    }  
    return true;
}

bool is_login_valid(char *UID, char *password) {
    return (is_UID(UID) && is_password(password));
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
