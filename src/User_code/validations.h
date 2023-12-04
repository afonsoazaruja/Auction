#ifndef VALIDATIONS_H
#define VALIDATIONS_H

#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define SIZE_UID 6
#define SIZE_PASSWORD 8
#define MAX_NAME_DESC 10
#define MAX_START_VAL 6
#define MAX_AUC_DURATION 5
#define MAX_FILENAME 28 // 24 chars + .xxx
#define MAX_FILESIZE 10000000

bool is_login_valid(char *UID, char *password);
bool is_UID(char *uid);
bool is_password(char *password);

bool is_open_valid(char *name, char *fname, char *start_value, char *timeactive);
bool is_desc_name(char *name);
bool is_start_val(char *start_value);
bool is_timeactive(char *timeactive);
bool is_filename(char *fname);

#endif 


