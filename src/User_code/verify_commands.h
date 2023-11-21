#ifndef VERIFY_COMMANDS_H
#define VERIFY_COMMANDS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifndef __cplusplus // To avoid conflict with C++ bool type
typedef enum {
    false = 0,
    true = 1
} bool;
#endif

bool command_selection(char *buffer, char (*msg)[128]);
bool verify_login(char *UID, char *PWD);
bool is_UID(char *str);
bool is_PWD(char *str);
bool verify_open(char words_command[10][20]);

#endif
