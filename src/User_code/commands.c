#include "user.h"
#include "commands.h"
#include "validations.h"

bool is_input_valid(char *buffer, int *socket_type, struct session *user) {
    char cmd[10];
    sscanf(buffer, "%s", cmd); // extract command
    *socket_type = SOCK_DGRAM; // assume its udp command
    // udp requests
    if (strcmp(cmd, "login") == 0) {
        return handle_login(buffer, user);
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
        } else if (strcmp(cmd, "mybids") == 0 || strcmp(cmd, "mb") == 0) {
            sprintf(buffer, "LMB %s\n", user->UID);
        }
        // tcp requests    
        else {
            *socket_type = SOCK_STREAM;
            if (strcmp(cmd, "open") == 0) {
                return handle_open(buffer, user);
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
        }
    } else { 
        sprintf(buffer, "user not logged in or invalid input");
        return false;
    } 
    return true;
}

bool handle_login(char *buffer, struct session *user) {
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
    }
    return true;
}

bool handle_open(char *buffer, struct session *user) {
    char name[MAX_NAME_DESC + 1];
    char start_value[MAX_START_VAL + 1];
    char timeactive[MAX_AUC_DURATION + 1];
    char asset_fname[MAX_FILENAME];

    if (sscanf(buffer, "%*s %s %s %s %s", name, asset_fname,
        start_value, timeactive) != 4) {
            sprintf(buffer, "invalid data for open");
            return false;
    }

    char *fname = get_file_name(asset_fname);

    if (!is_open_valid(name, fname, start_value, timeactive)) {
        free(fname);
        sprintf(buffer, "invalid data for open");
        return false;
    } else {
        long size = get_file_size(asset_fname);
        if (size > MAX_FILESIZE) {
            sprintf(buffer, "image too big");
            free(fname);
            return false;
        }
        sprintf(buffer, "OPA %s %s %s %s %ld", name, start_value, timeactive, asset_fname, size);
        free(fname);
    }
    return true;               
}

char* get_file_name(char *dir) {
    int len = strlen(dir);
    int a = len, b = 0;
    char *fname = malloc((len + 1)*sizeof(char));

    for(; a > 0; a--) {
        if(dir[a] == '/') {
            a++;
            break;
        }
    } for(; a < len; b++) {
        fname[b] = dir[a];
        a++;
    }
    fname[b] = '\0';
    return fname;
}

long get_file_size(char *fname) {
    struct stat filestat;
       
    if (stat(fname, &filestat) == 0) {
        return filestat.st_size;
    } else {
        perror("Error getting file information");
    }
    return 0; // only to compile
}

char *get_file_data(char *fname, long filesize) {
    char *data = malloc(filesize + 1);
    if (data == NULL) {
        perror("Error allocating memory");
        exit(1);
    }
    FILE *file = fopen(fname, "rb");
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }
    size_t bread = fread(data, 1, filesize, file);
    if (bread != (size_t)filesize) {
        perror("Error reading file");
        exit(1);
    }
    data[filesize] = '\0';

    fclose(file);
    return data;
}