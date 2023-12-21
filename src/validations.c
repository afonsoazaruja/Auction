#include "validations.h"

/* Checks if the words in buffer are seperated by a single space and if it ends with \n */
bool validate_buffer(const char *buffer) {
    size_t len = strlen(buffer);
    if (len == 0 || buffer[len - 1] != '\n') return false;
    
    for (size_t i = 0; i < len - 1; i++) 
        if (buffer[i] == ' ' && buffer[i + 1] == ' ') 
            // check if the 2+ spaces are between words
            for (int j=i+2; j < len; j++) 
                if (buffer[j] != ' ' && buffer[j] != '\n')
                    return false;       
    return true;
}

bool is_login_valid(char *UID, char *password) {
    return (is_UID(UID) && is_password(password));
}

bool is_AID(char *aid) {
    int len = strlen(aid);
    if (len != SIZE_AID) return false;

    for (int i = 0; i < len; i++) 
        if (!isdigit(aid[i])) return false;
    return true;
}

bool is_UID(char *uid) {
    int len = strlen(uid);
    if (len != SIZE_UID) return false;  

    for (int i = 0; i < len; i++) 
        if (!isdigit(uid[i])) return false;
    return true;
}

bool is_password(char *password) {  
    int len = strlen(password);
    if (len != SIZE_PASSWORD) return false;  

    for (int i = 0; i < len; i++) 
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
    int len = strlen(name);
    if (len > MAX_NAME_DESC) return false;  

    for (int i = 0; i < len; i++) 
        if (!isalnum(name[i]) && !(name[i] == '-') && !(name[i] == '_')) return false;
    return true;
}

bool is_start_val(char *start_value) {
    int len = strlen(start_value);
    if (len > MAX_START_VAL) return false;  

    for (int i = 0; i < len; i++) 
        if (!isdigit(start_value[i])) return false;
    return true;
}

bool is_timeactive(char *timeactive) {
    int len = strlen(timeactive);
    if (len > MAX_AUC_DURATION) return false;  

    for (int i = 0; i < len; i++) 
        if (!isdigit(timeactive[i])) return false;
    return true;
}

bool is_filename(char *fname) {
    int len = strlen(fname);
    if ((len > MAX_FILENAME) || len < MIN_FILENAME) return false;  

    // check for fname, excluding .xxx
    for (int i = 0; i < len - 4; i++) {
        if (isalnum(fname[i]) || fname[i] == '-' ||
             fname[i] == '_' || fname[i] == '.')
                continue;
            return false;
    }
    // check for .xxx
    if (fname[len - 4] != '.' || !isalpha(fname[len - 3]) ||
         !isalpha(fname[len - 2]) || !isalpha(fname[len - 1]))
         return false;
    return true;
}
