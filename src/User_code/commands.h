#ifndef VERIFY_COMMANDS_H
#define VERIFY_COMMANDS_H

#include "../validations.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <bits/types/FILE.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <sys/sendfile.h>
#include <fcntl.h>

#define MAX_CMD_SIZE 11

typedef struct session {
    bool logged;
    char UID[10];
    char password[10];
} session;

bool is_input_valid(char *buffer, int *socket_type, struct session *user);
bool is_login_valid(char *UID, char *password);
bool is_UID(char *uid);
bool is_password(char *password);
bool is_open_valid(char *name, char *fname, char *start_value, char *timeactive);
bool is_desc_name(char *name);
bool is_start_val(char *start_value);
bool is_timeactive(char *timeactive);
bool is_filename(char *filename);
bool handle_login(char *buffer, struct session *user);
bool handle_record(char *buffer);
bool handle_open(char *buffer, struct session *user);
bool handle_bid(char *buffer, struct session *user);
bool handle_close(char *buffer, struct session *user);
bool handle_asset(char *buffer);
long get_file_size(char *filename);
char *get_file_data(char *filename, long size);
char *get_file_name(char *dir);

#endif
