#ifndef VERIFY_COMMANDS_H
#define VERIFY_COMMANDS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <bits/types/FILE.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SIZE_UID 6
#define SIZE_PASSWORD 8
#define MAX_NAME_DESC 10
#define MAX_START_VAL 6
#define MAX_AUC_DURATION 5
#define MAX_FILENAME 28 // 24 chars + .xxx

#ifndef __cplusplus // To avoid conflict with C++ bool type
typedef enum {
    false = 0,
    true = 1
} bool;
#endif

typedef struct session {
    bool logged;
    char UID[10];
    char password[10];
} session;

bool is_input_valid(char *buffer, int *socket_type, struct session *user);
bool is_login_valid(char *UID, char *password);
bool is_UID(char *uid);
bool is_password(char *password);
bool is_open_valid(char *name, char *asset_fname, char *start_value, char *timeactive);
bool is_desc_name(char *name);
bool is_start_val(char *start_value);
bool is_timeactive(char *timeactive);
bool is_filename(char *filename);
char *get_file_size(char *filename);
char* long_to_string(long num);
char *get_file_data(char *filename);

#endif
