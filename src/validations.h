#ifndef VALIDATIONS_H
#define VALIDATIONS_H

#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#define SIZE_AID 3
#define SIZE_UID 6
#define SIZE_PASSWORD 8
#define MAX_NAME_DESC 10
#define MAX_BID 999999
#define MAX_START_VAL 6
#define MAX_AUC_DURATION 5
#define MAX_FILENAME 28 // 24 chars + .xxx
#define MIN_FILENAME 5 // 1 char + .xxx
#define MAX_FILESIZE 10 * 1024 * 1024

bool validate_buffer(const char *buffer);

bool is_login_valid(char *UID, char *password);
bool is_AID(char *aid);
bool is_UID(char *uid);
bool is_password(char *password);
bool is_bid(int value);

bool is_open_valid(char *name, char *fname, char *start_value, char *timeactive);
bool is_desc_name(char *name);
bool is_start_val(char *start_value);
bool is_timeactive(char *timeactive);
bool is_filename(char *fname);

#endif 


