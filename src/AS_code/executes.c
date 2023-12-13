#include "executes.h"
#include "aux_executes.h"

void ex_login(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID + 1];
    char password[SIZE_PASSWORD + 1];

    if (sscanf(request, "%*s %s %s", uid, password) != 2) exit(1);
    if (!is_login_valid(uid, password) || !validate_buffer(request)) {
        send_reply_to_user(fd, addr, "RLI ERR\n");
        return;
    }
    // if (is_logged_in(uid)) {
    //     send_reply_to_user(fd, addr, "user already logged in\n");
    //     return;
    // }
    if (is_registered(uid)) 
        try_to_login(fd, addr, uid, password);
    else 
        register_user(fd, addr, uid, password);
}

void ex_logout(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID + 1];
    char password[SIZE_PASSWORD + 1];
    char dir[100];

    if (sscanf(request, "%*s %s %s", uid, password) != 2) exit(1);
    if (!is_UID(uid) || !is_password(password) || !validate_buffer(request)) {
        send_reply_to_user(fd, addr, "RLO ERR\n");
        return;
    }
    if (!is_registered(uid)) {
        send_reply_to_user(fd, addr, "RLO UNR\n");
        return;
    }
    else if (!is_logged_in(uid)) {
        send_reply_to_user(fd, addr, "RLO NOK\n");
        return;
    }
    sprintf(dir, "%s/%s/%s_login.txt", USERS_DIR, uid, uid);
    if (remove(dir) == 0) {
        send_reply_to_user(fd, addr, "RLO OK\n");
    } else {
        perror("Error removing file"); exit(1);
    }
}

void ex_unregister(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID + 1];
    char password[SIZE_PASSWORD + 1];
    char login_dir[100];
    char pass_dir[100];

    if (sscanf(request, "%*s %s %s", uid, password) != 2) exit(1);
    if (!is_UID(uid) || !is_password(password) || !validate_buffer(request)) {
        send_reply_to_user(fd, addr, "RUR ERR\n");
        return;
    }    
    if (!is_registered(uid)) {
        send_reply_to_user(fd, addr, "RUR UNR\n");
        return;
    }
    else if (!is_logged_in(uid)) {
        send_reply_to_user(fd, addr, "RUR NOK\n");
        return;
    }

    sprintf(login_dir, "%s/%s/%s_login.txt", USERS_DIR, uid, uid);
    sprintf(pass_dir, "%s/%s/%s_pass.txt", USERS_DIR, uid, uid);
    if (remove(login_dir) == 0 && remove(pass_dir) == 0) {
       send_reply_to_user(fd, addr, "RUR OK\n");
    } else {
        perror("Error removing file"); exit(1);
    }
}

void ex_myauctions(int fd, struct sockaddr_in addr, char *request) {
    // To do
    send_reply_to_user(fd, addr, "TST NOP\n");
}
void ex_mybids(int fd, struct sockaddr_in addr, char *request) {
    // To do
    send_reply_to_user(fd, addr, "TST NOP\n");
}
void ex_list(int fd, struct sockaddr_in addr, char *request) {
    // To do
    send_reply_to_user(fd, addr, "TST NOP\n");
}
void ex_show_record(int fd, struct sockaddr_in addr, char *request) {
    // To do
    send_reply_to_user(fd, addr, "TST NOP\n");
}

void ex_open(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID+1];
    char password[SIZE_PASSWORD+1];
    char name[MAX_NAME_DESC+1];
    char start_value[MAX_START_VAL+1];
    char timeactive[MAX_AUC_DURATION+1];
    char asset_fname[MAX_FILENAME+1];
    long size = 0;
    char asset_dir[100];
    char *aid = get_aid();

    if (strcmp(aid, "000") == 0) {
        free(aid); send_reply_to_user(fd, addr, "ROA NOK\n"); 
        return;
    }

    sscanf(request, "%*s %s %s %s %s %s %s %ld", uid, password, name,
    start_value, timeactive, asset_fname, &size);

    if (!is_logged_in(uid)) {
        send_reply_to_user(fd, addr, "ROA NLG\n"); return;
    }
    if (!is_open_valid(name, asset_fname, start_value, timeactive)) {
        send_reply_to_user(fd, addr, "ROA NOK\n"); return;
    }

    register_auction(fd, addr, uid, name, asset_fname, start_value, timeactive, aid);
    // asset
    sprintf(asset_dir, "%s/%s/ASSET/%s", AUCTIONS_DIR, aid, asset_fname);
    char *data = malloc(size);
    FILE *file = fopen(asset_dir, "wb");
    if (file == NULL) {
        perror("Error opening file"); exit(1);
    }
    ssize_t n = 0;
    do { // read bytes of file and write
        n = recv(fd, data, size, 0);
        if(n==-1) exit(1);
        size_t bytes_written = fwrite(data, 1, n, file);
        if (bytes_written != n) {
            fclose(file);
            perror("Error writing to file"); exit(1);
        }
        size -= n;
    } while(size != 0);

    sprintf(request, "ROA OK %s\n", aid);
    free(aid);
    send_reply_to_user(fd, addr, request);
}

void ex_close(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID+1];
    char pwd[SIZE_PASSWORD+1];
    char aid[MAX_STATUS_SIZE+1];
    char dir[100];

    sscanf(request, "%*s %s %s %s", uid, pwd, aid);

    if (!is_correct_password(pwd, uid) || !is_registered(uid)) {
        send_reply_to_user(fd, addr, "RCL NOK\n"); return;
    }
    if (!is_logged_in(uid)) {
        send_reply_to_user(fd, addr, "RCL NLG\n"); return;
    }
    sprintf(dir, "%s/%s", AUCTIONS_DIR, aid); // aid exists
    if (!directoryExists(dir)) {
        send_reply_to_user(fd, addr, "RCL EAU\n"); return;
    }
    if (!is_auction_owned(uid, aid)) {
        send_reply_to_user(fd, addr, "RCL EOW\n"); return;
    }
    if (!is_auction_active(aid)) {
        send_reply_to_user(fd, addr, "RCL END\n"); return;
    }
    // To do
    send_reply_to_user(fd, addr, "TST NOP\n");
}

void ex_show_asset(int fd, struct sockaddr_in addr, char *request) {
    char aid[MAX_STATUS_SIZE+1];
    sscanf(request, "%*s %s", aid);

    // To do
    send_reply_to_user(fd, addr, "TST NOP\n");
}

void ex_bid(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID+1];
    char password[SIZE_PASSWORD+1];
    char aid[MAX_STATUS_SIZE+1];
    char auction_dir[100];
    int value;

    sscanf(request, "%*s %s %s %s %d", uid, password, aid, &value);
    
    if (!is_auction_active(aid)) {
        send_reply_to_user(fd, addr, "RBD NOK\n");
        return;
    }

    // To do
    send_reply_to_user(fd, addr, "TST NOP\n");
}
