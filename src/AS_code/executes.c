#include "executes.h"
#include "aux_executes.h"

void ex_login(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID + 1];
    char password[SIZE_PASSWORD + 1];
    char path_user_dir[100];

    if (sscanf(request, "%*s %s %s", uid, password) != 2) exit(1);
    if (!is_login_valid(uid, password) || !validate_buffer(request)) {
        send_reply_to_user(fd, addr, "RLI ERR\n");
        return;
    }
    build_path_user_dir(path_user_dir, uid);

    // if (is_logged_in(path_user_dir, uid)) {
    //     send_reply_to_user(fd, addr, "user already logged in\n");
    //     return;
    // }
    if (is_registered(path_user_dir, uid)) 
        try_to_login(fd, addr, path_user_dir, uid, password);
    else 
        register_user(fd, addr, path_user_dir, uid, password);
}

void ex_logout(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID + 1];
    char password[SIZE_PASSWORD + 1];
    char path_user_dir[100];

    if (sscanf(request, "%*s %s %s", uid, password) != 2) exit(1);
    if (!is_UID(uid) || !is_password(password) || !validate_buffer(request)) {
        send_reply_to_user(fd, addr, "RLO ERR\n");
        return;
    }
    build_path_user_dir(path_user_dir, uid);

    if (!is_registered(path_user_dir, uid)) {
        send_reply_to_user(fd, addr, "RLO UNR\n");
        return;
    }
    else if (!is_logged_in(path_user_dir, uid)) {
        send_reply_to_user(fd, addr, "RLO NOK\n");
        return;
    }
    char *login_file_name = get_file_name(path_user_dir, uid, "_login", ".txt");

    if (remove(login_file_name) == 0) {
       free(login_file_name);
       send_reply_to_user(fd, addr, "RLO OK\n");
    } else {
        free(login_file_name);
        perror("Error removing file"); exit(1);
    }
}

void ex_unregister(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID + 1];
    char password[SIZE_PASSWORD + 1];
    char path_user_dir[100];

    if (sscanf(request, "%*s %s %s", uid, password) != 2) exit(1);
    if (!is_UID(uid) || !is_password(password) || !validate_buffer(request)) {
        send_reply_to_user(fd, addr, "RUR ERR\n");
        return;
    }    
    build_path_user_dir(path_user_dir, uid);

    if (!is_registered(path_user_dir, uid)) {
        send_reply_to_user(fd, addr, "RUR UNR\n");
        return;
    }
    else if (!is_logged_in(path_user_dir, uid)) {
        send_reply_to_user(fd, addr, "RUR NOK\n");
        return;
    }
    char *login_file_name = get_file_name(path_user_dir, uid, "_login", ".txt");
    char *pass_file_name = get_file_name(path_user_dir, uid, "_pass", ".txt");

    if (remove(login_file_name) == 0 && remove(pass_file_name) == 0) {
       free(login_file_name);
       free(pass_file_name);
       send_reply_to_user(fd, addr, "RUR OK\n");
    } else {
        free(login_file_name);
        free(pass_file_name);
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
    char user_dir[100];
    char asset_dir[100];
    char aid[4];

    sscanf(request, "%*s %s %s %s %s %s %s %ld", uid, password, name,
    start_value, timeactive, asset_fname, &size);

    build_path_user_dir(user_dir, uid);
    if (!is_logged_in(user_dir, uid)) {
        send_reply_to_user(fd, addr, "ROA NLG\n");
        return;
    }
    if (!is_open_valid(name, asset_fname, start_value, timeactive)) {
        send_reply_to_user(fd, addr, "ROA NOK\n");
        return;
    }
    num_aid++; format_aid(aid);

    register_auction(fd, addr, uid, name, asset_fname, start_value, timeactive, aid);
    build_auction_asset_dir(asset_dir, aid);
    char *asset_file = get_file_name(asset_dir, asset_fname, "", "");

    char *data = malloc(size);
    FILE *file = fopen(asset_file, "wb");
    if (file == NULL) {
        perror("Error opening file"); exit(1);
    }
    ssize_t n = 0;
    do { // read bytes of file and write
        recv(fd, data, size, 0);
        if(n==-1) exit(1);
        size_t bytes_written = fwrite(data, 1, n, file);
        if (bytes_written != n) {
            fclose(file);
            perror("Error writing to file"); exit(1);
        }
        size -= n;
    } while(size != 0);

    sprintf(request, "ROA OK %s\n", aid);
    send_reply_to_user(fd, addr, request);
}

void ex_close(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID+1];
    char pwd[SIZE_PASSWORD+1];
    char aid[MAX_STATUS_SIZE+1];
    sscanf(request, "%*s %s %s %s", uid, pwd, aid);

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
    build_path_auction_dir(auction_dir, aid);

    if (!is_auction_active(auction_dir, aid)) {
        send_reply_to_user(fd, addr, "RBD NOK\n");
        return;
    }

}
