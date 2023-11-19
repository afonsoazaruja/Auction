#include "verify_commands.h"
#include <sys/types.h>

 bool is_command_valid(char *command) {
    char words_command[10][20] = {0};  
    int j = 0, k = 0;

    // extract words from command 
    for (int i=0; command[i]!='\n'; i++) {
        printf("%c\n", command[i]);
        if (command[i] != ' ') {
            words_command[j][k] = command[i];
            k++;
        } else {
            words_command[j][k] = '\0';
            j++;
            k = 0;
        }
    }

    if ((!strcmp(words_command[0], "exit") ||
    (!strcmp(words_command[0], "logout")) ||
    (!strcmp(words_command[0], "unregister")) ||
    (!strcmp(words_command[0], "myauctions")) ||
    (!strcmp(words_command[0], "ma")) || 
    (!strcmp(words_command[0], "mybids")) ||
    (!strcmp(words_command[0], "mb")) || 
    (!strcmp(words_command[0], "list")) || 
    (!strcmp(words_command[0], "l"))) && strlen(words_command[1]) == 0) return true;
    else if (!strcmp(words_command[0], "login")) return verify_login(words_command);
    else if (!strcmp(words_command[0], "open")) return verify_open(words_command);

    return false;
 }

bool verify_login(char words_command[10][20]) {
    puts("VERIFY LOGIN WAS RUNNED");
    if (words_command[1] == NULL) return false;
    puts("PASSED FIRST LOGIN TEST");
    if (is_UID(words_command[1]) && is_password(words_command[2])) return true;
    puts("FAILED SECOND LOGIN TEST"); 
    if (is_password(words_command[1]) && strlen(words_command[2]) == 0) return true;
    puts("FAILED THIRD LOGIN TEST"); 
    
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
