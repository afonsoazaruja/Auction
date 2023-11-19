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

bool is_command_valid(char words_command[10][20]);
bool verify_login(char words_command[10][20]);
bool is_UID(char *str);
bool is_password(char *str);
bool verify_open(char words_command[10][20]);

#endif
