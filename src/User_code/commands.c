#include "commands.h"

bool is_input_valid(char *buffer, int *socket_type, struct session *user) {
    char cmd[10];
    sscanf(buffer, "%s", cmd); // extract command
    *socket_type = SOCK_DGRAM; // assume its udp command

    // udp requests
    if (strcmp(cmd, "login") == 0) {
        char uid[SIZE_UID + 1], password[SIZE_PASSWORD + 1];
        if (sscanf(buffer, "%*s %s %s", uid, password) != 2 ||
            !is_login_valid(uid, password)) {
            sprintf(buffer, "invalid UID or password");
            return false;
        } 
        else if (user->logged == true) {
            sprintf(buffer, "user already logged in");
            return false;
        }
        else {
            sprintf(buffer, "LIN %s %s\n", uid, password);
            strcpy(user->UID, uid);
            strcpy(user->password, password);
            user->logged = true;
        }
    } else if (strcmp(cmd, "list") == 0 || strcmp(cmd, "l") == 0) {
        sprintf(buffer, "LST\n");
    } else if (strcmp(cmd, "exit") == 0) {
        sprintf(buffer, "EXT\n");
    } else if (strcmp(cmd, "show_record") == 0 || strcmp(cmd, "sr") == 0) {
        sprintf(buffer, "SRC\n");
    }     

    else if (user->logged == true) {
        if (strcmp(cmd, "logout") == 0) {
            sprintf(buffer, "LOU %s %s\n", user->UID, user->password);
            user->logged = false;
        } else if (strcmp(cmd, "unregister") == 0) {
            sprintf(buffer, "UNR %s %s\n", user->UID, user->password);
        } else if (strcmp(cmd, "myauctions") == 0 || strcmp(cmd, "ma") == 0) {
            sprintf(buffer, "LMA %s\n", user->UID);
        } else if (strcmp(cmd, "my bids") == 0 || strcmp(cmd, "mb") == 0) {
            sprintf(buffer, "LMB %s\n", user->UID);
        }
        // tcp requests    
        else {
            if (strcmp(cmd, "open") == 0) {
                char name[MAX_NAME_DESC + 1];
                char start_value[MAX_START_VAL + 1];
                char timeactive[MAX_AUC_DURATION + 1];
                char asset_fname[MAX_FILENAME];
                if (sscanf(buffer, "%*s %s %s %s %s", name, asset_fname,
                    start_value, timeactive) != 4 ||
                    !is_open_valid(name, asset_fname, start_value, timeactive))
                    sprintf(buffer, "invalid data for open");
                else {
                    long size = get_file_size(asset_fname);
                    char *data = get_file_data(asset_fname, size);
                    if (size > MAX_FILESIZE) {
                        sprintf(buffer, "image too big");
                        free(data);
                        return false;
                    }
                    sprintf(buffer, "OPA %s %s %s %s %s %s %ld %s\n", user->UID, user->password, 
                    name, start_value, timeactive, asset_fname, size, data); 
                    free(data);               
                }
            } else if (strcmp(cmd, "close") == 0) {
                sprintf(buffer, "CLS %s %s\n", user->UID, user->password);
            } else if (strcmp(cmd, "show_asset") == 0) {
                sprintf(buffer, "SAS\n");
            } else if (strcmp(cmd, "bid") == 0) {
                sprintf(buffer, "BID %s %s\n", user->UID, user->password);
            } else {
                sprintf(buffer, "Invalid input");
                return false;
            }
            *socket_type = SOCK_STREAM;
        }
    }
    else { // user not logged in
        sprintf(buffer, "User not logged in");
        return false;
    } 
    return true;
}

long get_file_size(char *filename) {
    struct stat filestat;
       
    if (stat(filename, &filestat) == 0) {
        return filestat.st_size;
    } else {
        perror("Error getting file information");
    }
    return 0; // only to compile
}

char *get_file_data(char *filename, long filesize) {
    char *data = malloc(sizeof(filesize));

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }
    fread(data, 1, filesize, file);

    fclose(file);
    return data; // only here so we can compile...
}

bool is_login_valid(char *UID, char *password) {
    return (is_UID(UID) && is_password(password));
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

bool is_open_valid(char *name, char *asset_fname, char *start_value, char *timeactive) {
    return (is_desc_name(name) && is_start_val(start_value) &&
     is_timeactive(timeactive) && is_filename(asset_fname));
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
bool is_filename(char *filename) {
    int length = strlen(filename);
    if (length > MAX_FILENAME) return false;  

    // check for filename, excluding .xxx
    for (int i = 0; i < length - 4; i++) 
        if (isalnum(filename[i]) || filename[i] == '-' ||
             filename[i] == '_' || filename[i] == '.')
                continue;
            return false;
    // check for .xxx
    if (filename[length - 4] != '.' || !isalpha(filename[length - 3]) ||
         !isalpha(filename[length - 2]) || !isalpha(filename[length - 1]))
         return false;
    return true;
}

