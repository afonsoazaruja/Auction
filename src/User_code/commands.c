#include "user.h"
#include "commands.h"

bool is_input_valid(char *buffer, int *socket_type, struct session *user) {
    char cmd[MAX_CMD_SIZE+1] = "";
    if (sscanf(buffer, "%11s", cmd) != 1) { // extract command
        if (cmd[0] == '\0') {
            sprintf(buffer, "invalid input");
            return false;
        } 
        sprintf(buffer, "sscanf could not read input"); return false;
    }
    *socket_type = SOCK_DGRAM; // assume its udp command
    if (!validate_buffer(buffer)) {
        sprintf(buffer, "two or more spaces between words");
        return false;
    }
    // udp requests
    if (strcmp(cmd, "login") == 0) {
        return handle_login(buffer, user);
    } else if (strcmp(cmd, "list") == 0 || strcmp(cmd, "l") == 0) {
        sprintf(buffer, "LST\n");
    } else if (strcmp(cmd, "exit") == 0) {
        sprintf(buffer, "EXT\n");
    } else if (strcmp(cmd, "show_record") == 0 || strcmp(cmd, "sr") == 0) {
        return handle_record(buffer);
    } else if (user->logged == true || strcmp(cmd, "show_asset") == 0 ||
     (strcmp(cmd, "sa") == 0)) {
        // user must be logged in
        if (strcmp(cmd, "logout") == 0) {
            sprintf(buffer, "LOU %s %s\n", user->UID, user->password);
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
                return handle_close(buffer, user);
            } else if ((strcmp(cmd, "show_asset") == 0) || (strcmp(cmd, "sa") == 0)) {
                return handle_asset(buffer);
            } else if ((strcmp(cmd, "bid") == 0) || (strcmp(cmd, "b") == 0)) {
                return handle_bid(buffer, user);
            } else {
                sprintf(buffer, "invalid input");
                return false;
            }
        } 
    }
    else {
        sprintf(buffer, "user not logged in or invalid input");
        return false;
    }   
    return true;
}

bool handle_login(char *buffer, struct session *user) {
    char uid[SIZE_UID+1], password[SIZE_PASSWORD+1];

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

bool handle_record(char *buffer) {
    char aid[SIZE_AID+1];
    
    if (sscanf(buffer, "%*s %s", aid) != 1) {
        sprintf(buffer, "missing AID");
    }
    else if (!is_AID(aid)) {
        sprintf(buffer, "invalid AID");
    }
    else {
        sprintf(buffer, "SRC %s\n", aid);
        return true;
    }
    return false;
}

bool handle_open(char *buffer, struct session *user) {
    char name[MAX_NAME_DESC+1];
    char start_value[MAX_START_VAL+1];
    char timeactive[MAX_AUC_DURATION+1];
    char asset_fname[MAX_FILENAME+1];
    char asset_dir[14+MAX_FILENAME+1];

    if (sscanf(buffer, "%*s %s %s %s %s", name, asset_fname,
        start_value, timeactive) != 4) {
        sprintf(buffer, "invalid data for open");
        return false;
    }
    if (!is_open_valid(name, asset_fname, start_value, timeactive)) {
        sprintf(buffer, "invalid data for open");
        return false;
    } else {
        sprintf(asset_dir, "%s/%s", ASSET_DIR, asset_fname);
        long size = get_file_size(asset_dir);
        if (size > MAX_FILESIZE) {
            sprintf(buffer, "image too big");
            return false;
        }
        sprintf(buffer, "OPA %s %s %s %s %s %s %ld ", 
        user->UID, user->password, name, start_value, timeactive, asset_fname, size);
    }
    return true;               
}

bool handle_asset(char *buffer) {
    char aid[SIZE_AID+1];
    
    if (sscanf(buffer, "%*s %s", aid) != 1) {
        sprintf(buffer, "missing AID");
    }
    else if (!is_AID(aid)) {
        sprintf(buffer, "invalid AID");
    }
    else {
        sprintf(buffer, "SAS %s\n", aid);
        return true;
    }
    return false;
}

bool handle_bid(char *buffer, struct session *user) {
    int value = 0;
    char aid[SIZE_AID+1];
    
    if (sscanf(buffer, "%*s %s %d", aid, &value) != 2) {
        sprintf(buffer, "missing AID or bid");
    }
    else if (!is_AID(aid)) {
        sprintf(buffer, "invalid AID");
    }
    else if (!is_bid(value)) {
        sprintf(buffer, "max bid");
    }
    else {
        sprintf(buffer, "BID %s %s %s %d\n", user->UID, user->password, aid, value);
        return true;
    }
    return false;
}

bool handle_close(char *buffer, struct session *user) {
    char aid[SIZE_AID+1];
    
    if (sscanf(buffer, "%*s %s", aid) != 1) {
        sprintf(buffer, "missing AID");
    }
    if (!is_AID(aid)) {
        sprintf(buffer, "invalid AID");
    }
    else {
        sprintf(buffer, "CLS %s %s %s\n", user->UID, user->password, aid);
        return true;
    }
    return false;
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
       
    if (stat(fname, &filestat) != 0) {
        perror("Error getting file information"); return -1;
    } 
    return filestat.st_size;
}

void display_help() {
    printf("\n");
    printf("%-48s %s\n", "Command", "Description");
    printf("---------------------------------------------    --------------------------------------------------------------\n");
    printf("%-48s %s\n", "login UID password", "Log in or register a user");
    printf("%-48s %s\n", "logout", "Log out the currently logged-in user");
    printf("%-48s %s\n", "unregister", "Unregister the currently logged-in user");
    printf("%-48s %s\n", "exit", "Exit the User Application");
    printf("%-48s %s\n", "open name asset_fname start_value timeactive", "Open a new auction");
    printf("%-48s %s\n", "close AID", "Close an ongoing auction");
    printf("%-48s %s\n", "myauctions or ma", "List auctions started by the logged-in user");
    printf("%-48s %s\n", "mybids or mb", "List auctions for which the logged-in user has placed a bid");
    printf("%-48s %s\n", "list or l", "List all auctions");
    printf("%-48s %s\n", "show_asset AID or sa AID", "View the image file of the asset in sale");
    printf("%-48s %s\n", "bid AID value or b AID value", "Place a bid for an auction");
    printf("%-48s %s\n", "show_record AID or sr AID", "View the record of an auction");
    printf("\n");
}
