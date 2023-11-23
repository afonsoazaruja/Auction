#include "verify_commands.h"
#include <sys/types.h>

bool is_input_valid(char *buffer, int *socket_type) {
    char cmd[10];
    sscanf(buffer, "%s", cmd); // extract command

    // udp requests
    if (strcmp(cmd, "login") == 0) {
        char uid[128], password[128];
        if (sscanf(buffer, "%*s %s %s", uid, password) != 2 || !is_login_valid(uid, password)) {
            sprintf(buffer, "invalid UID or password");
            return false;
        } else {
            sprintf(buffer, "LIN %s %s\n", uid, password);
            *socket_type = SOCK_DGRAM;
        }
    } else if (strcmp(cmd, "logout") == 0) {
        sprintf(buffer, "LOU\n");
        *socket_type = SOCK_DGRAM;
    } else if (strcmp(cmd, "unregister") == 0) {
        sprintf(buffer, "UNR\n");
        *socket_type = SOCK_DGRAM;
    } else if (strcmp(cmd, "myauctions") == 0 || strcmp(cmd, "ma") == 0) {
        sprintf(buffer, "LMA\n");
        *socket_type = SOCK_DGRAM;
    } else if (strcmp(cmd, "my bids") == 0 || strcmp(cmd, "mb") == 0) {
        sprintf(buffer, "LMB\n");
        *socket_type = SOCK_DGRAM;
    } else if (strcmp(cmd, "list") == 0 || strcmp(cmd, "l") == 0) {
        sprintf(buffer, "LST\n");
        *socket_type = SOCK_DGRAM;
    } else if (strcmp(cmd, "exit") == 0) {
        sprintf(buffer, "EXT\n");
        *socket_type = SOCK_DGRAM;
    } else if (strcmp(cmd, "show_record") == 0 || strcmp(cmd, "sr") == 0) {
        sprintf(buffer, "SRC\n");
        *socket_type = SOCK_DGRAM;
    // tcp requests    
    } else if (strcmp(cmd, "open") == 0) {
        sprintf(buffer, "OPA\n");
        *socket_type = SOCK_STREAM;
    } else if (strcmp(cmd, "close") == 0) {
        sprintf(buffer, "CLS\n");
        *socket_type = SOCK_STREAM; 
    } else if (strcmp(cmd, "show_asset") == 0) {
        sprintf(buffer, "SAS\n");
        *socket_type = SOCK_STREAM;
    } else if (strcmp(cmd, "bid") == 0) {
        sprintf(buffer, "BID\n");
        *socket_type = SOCK_STREAM;
    } else {
        sprintf(buffer, "Invalid input");
        return false;
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
