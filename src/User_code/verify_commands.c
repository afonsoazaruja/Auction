#include "verify_commands.h"
#include <sys/types.h>

 bool is_command_valid(char words_command[10][20]) {
    if (!strcmp(words_command[0], "exit"))
        return true;
    else if (!strcmp(words_command[0], "logout"))
        strcpy(words_command[0], "LOU");
    else if (!strcmp(words_command[0], "unregister"))
        strcpy(words_command[0], "UNR");
    else if (!strcmp(words_command[0], "myauctions") || 
    !strcmp(words_command[0], "ma"))
        strcpy(words_command[0], "LMA");
    else if (!strcmp(words_command[0], "mybids") ||
    !strcmp(words_command[0], "mb"))
        strcpy(words_command[0], "LMB");
    else if (!strcmp(words_command[0], "list") ||
    !strcmp(words_command[0], "l"))
        strcpy(words_command[0], "LST");
    else if (!strcmp(words_command[0], "login")) {
        strcpy(words_command[0], "LOU");
        return verify_login(words_command);
    }
    else if (!strcmp(words_command[0], "open"))
        return verify_open(words_command);
    else
        return false;
    return true;
 }

bool verify_login(char words_command[10][20]) {
    if (words_command[1] == NULL) return false;
    if (is_UID(words_command[1]) && is_password(words_command[2])) return true;
    if (is_password(words_command[1]) && strlen(words_command[2]) == 0) return true;
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
    puts("PASSED PASS LENGTH TEST");

    for (int i = 0; i < length; i++) 
        if (!isalnum(str[i])) return false;
    puts("PASSED IS_PASSWORD TEST");
    return true;
}

bool verify_open(char words_command[10][20]) {
    // TO DO
    return false;
}
