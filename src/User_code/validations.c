
#include "validations.h"

bool is_login_valid(char *UID, char *password) {
    return (is_UID(UID) && is_password(password));
}

bool is_AID(char *aid) {
    int length = strlen(aid);
    if (length != SIZE_AID) return false;

    for (int i = 0; i < length; i++) 
        if (!isdigit(aid[i])) return false;
    return true;
}

bool is_UID(char *uid) {
    int length = strlen(uid);
    if (length != SIZE_UID) return false;  

    for (int i = 0; i < length; i++) 
        if (!isdigit(uid[i])) return false;
    return true;
}

bool is_password(char *password) {  
    int length = strlen(password);
    if (length != SIZE_PASSWORD) return false;  

    for (int i = 0; i < length; i++) 
        if (!isalnum(password[i])) return false;
    return true;
}

bool is_bid(int value) {
    if (value > MAX_BID) return false;
    return true;
}

bool is_open_valid(char *name, char *fname, char *start_value, char *timeactive) {
    return (is_desc_name(name) && is_start_val(start_value) &&
     is_timeactive(timeactive) && is_filename(fname));
}

bool is_desc_name(char *name) {
    int length = strlen(name);
    if (length > MAX_NAME_DESC) return false;  

    for (int i = 0; i < length; i++) 
        if (!isalnum(name[i])) return false;
    return true;
}

bool is_start_val(char *start_value) {
    int length = strlen(start_value);
    if (length > MAX_START_VAL) return false;  

    for (int i = 0; i < length; i++) 
        if (!isdigit(start_value[i])) return false;
    return true;
}

bool is_timeactive(char *timeactive) {
    int length = strlen(timeactive);
    if (length > MAX_AUC_DURATION) return false;  

    for (int i = 0; i < length; i++) 
        if (!isdigit(timeactive[i])) return false;
    return true;
}

bool is_filename(char *fname) {
    int length = strlen(fname);
    if ((length > MAX_FILENAME) || length < MIN_FILENAME) return false;  

    // check for fname, excluding .xxx
    for (int i = 0; i < length - 4; i++) {
        if (isalnum(fname[i]) || fname[i] == '-' ||
             fname[i] == '_' || fname[i] == '.')
                continue;
            return false;
    }
    // check for .xxx
    if (fname[length - 4] != '.' || !isalpha(fname[length - 3]) ||
         !isalpha(fname[length - 2]) || !isalpha(fname[length - 1]))
         return false;
    return true;
}
