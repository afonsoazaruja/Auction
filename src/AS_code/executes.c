#include "executes.h"
#include "aux_executes.h"
#include "requests.h"

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
    char login_file_name[100];

    if (sscanf(request, "%*s %s %s", uid, password) != 2) exit(1);

    if (!is_UID(uid) || !is_password(password) || !validate_buffer(request)) {
        send_reply_to_user(fd, addr, "RLO ERR\n");
    }
    else if (!is_registered(uid)) {
        send_reply_to_user(fd, addr, "RLO UNR\n");
    }
    else if (!is_logged_in(uid)) {
        send_reply_to_user(fd, addr, "RLO NOK\n");
    }
    else {
        sprintf(login_file_name, "%s/%s/%s_login.txt", PATH_USERS_DIR, uid, uid);
        if (remove(login_file_name) == 0) {
            send_reply_to_user(fd, addr, "RLO OK\n");
        } else {
            perror("Error removing file"); exit(1);
        }
    }
}

void ex_unregister(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID + 1];
    char password[SIZE_PASSWORD + 1];
    char login_file_name[100];
    char pass_file_name[100];

    if (sscanf(request, "%*s %s %s", uid, password) != 2) exit(1);

    if (!is_UID(uid) || !is_password(password) || !validate_buffer(request)) {
        send_reply_to_user(fd, addr, "RUR ERR\n");
    }    
    else if (!is_registered(uid)) {
        send_reply_to_user(fd, addr, "RUR UNR\n");
    }
    else if (!is_logged_in(uid)) {
        send_reply_to_user(fd, addr, "RUR NOK\n");
    }
    else {
        sprintf(login_file_name, "%s/%s/%s_login.txt", PATH_USERS_DIR, uid, uid);
        sprintf(pass_file_name, "%s/%s/%s_pass.txt", PATH_USERS_DIR, uid, uid);

        if (remove(login_file_name) == 0 && remove(pass_file_name) == 0) {
            send_reply_to_user(fd, addr, "RUR OK\n");
        } else {
            perror("Error removing file"); exit(1);
        }
    }
}

void ex_myauctions(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID+1];
    sscanf(request, "%*s %s", uid);

    if (!has_started_auctions(uid)) {
        send_reply_to_user(fd, addr, "RMA NOK\n");
    }
    if (!is_logged_in(uid)) {
        send_reply_to_user(fd, addr, "RMA NLG\n");
    }
    else {
        char hosted_path[100];
        sprintf(hosted_path, "%s/%s/HOSTED", PATH_USERS_DIR, uid);
        char *auctions_list = create_list_auctions(hosted_path);

        char reply[strlen(auctions_list) + 10];
        sprintf(reply, "RMA OK %s\n", auctions_list);
        send_reply_to_user(fd, addr, reply);
    }
}

void ex_mybids(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID+1];
    sscanf(request, "%*s %s", uid);

    if (!has_placed_bids(uid)) {
        send_reply_to_user(fd, addr, "RMB NOK\n");
    }
    else if (!is_logged_in(uid)) {
        send_reply_to_user(fd, addr, "RMB NLG\n");
    }
    else {
        char bidded_path[100];
        sprintf(bidded_path, "%s/%s/BIDDED", PATH_USERS_DIR, uid);
        char *auctions_list = create_list_auctions(bidded_path);

        char reply[strlen(auctions_list) + 10];
        sprintf(reply, "RMB OK %s\n", auctions_list);
        send_reply_to_user(fd, addr, reply);
    }
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
    sprintf(asset_dir, "%s/%s/ASSET/%s", PATH_AUCTIONS_DIR, aid, asset_fname);
    char *data = malloc(size);
    FILE *file = fopen(asset_dir, "wb");
    if (file == NULL) {
        perror("Error opening file"); fclose(file); exit(1);
    }
    ssize_t n = 0;
    do { // read bytes of file and write
        n = recv(fd, data, size, 0);
        if(n==-1) exit(1);
        size_t bytes_written = fwrite(data, 1, n, file);
        if (bytes_written != n) {
            perror("Error writing to file"); fclose(file); exit(1);
        }
        size -= n;
    } while (size != 0);

    sprintf(request, "ROA OK %s\n", aid);
    send_reply_to_user(fd, addr, request);
    free(aid);
    fclose(file);
}

void ex_close(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID+1];
    char password[SIZE_PASSWORD+1];
    char aid[MAX_STATUS_SIZE+1];
    char dir[100];

    sscanf(request, "%*s %s %s %s", uid, password, aid);
    sprintf(dir, "%s/%s", PATH_AUCTIONS_DIR, aid); 

    if (!directoryExists(dir)) {
        send_reply_to_user(fd, addr, "RCL EAU\n"); 
    }
    else if (!is_correct_password(password, uid) || !is_registered(uid)) {
        send_reply_to_user(fd, addr, "RCL NOK\n"); 
    }
    else if (!is_logged_in(uid)) {
        send_reply_to_user(fd, addr, "RCL NLG\n"); 
    }
    else if (!is_auction_owned(uid, aid)) {
        send_reply_to_user(fd, addr, "RCL EOW\n"); 
    }
    else if (!is_auction_active(aid)) {
        send_reply_to_user(fd, addr, "RCL END\n"); 
    }
    else {
        close_auction(aid);
        send_reply_to_user(fd, addr, "RCL OK\n");
    }
}

void ex_show_asset(int fd, struct sockaddr_in addr, char *request) {
    char aid[MAX_STATUS_SIZE+1], directory[100], asset_fname[MAX_FILENAME], reply[MAX_BUFFER_SIZE];
    ssize_t size;
    sscanf(request, "%*s %s", aid);
    
    sprintf(directory, "%s/%s", PATH_AUCTIONS_DIR, aid);
    if (!directoryExists(directory)) {
        send_reply_to_user(fd, addr, "RSA NOK\n"); return;
    }
    // read START_aid.txt for asset name
    sprintf(directory, "%s/%s/START_%s.txt", PATH_AUCTIONS_DIR, aid, aid);
    FILE *start_file = fopen(directory, "r");
    if (start_file != NULL) {
        fscanf(start_file, "%*s %*s %s", asset_fname);
        fclose(start_file);
    } else {
        perror("Error opening file"); exit(1);
        send_reply_to_user(fd, addr, "RSA NOK\n"); return;
    }
    // send asset
    sprintf(directory, "%s/%s/ASSET/%s", PATH_AUCTIONS_DIR, aid, asset_fname);
    size = get_file_size(directory);
    int asset_fd = open(directory, O_RDONLY);

    sprintf(reply, "RSA OK %s %ld ", asset_fname, size);
    send_reply_to_user(fd, addr, reply);
    off_t offset = 0;
    while (size > 0) {
        ssize_t sent_bytes = sendfile(fd, asset_fd, &offset, size);
        if (sent_bytes == -1) {
            perror("Error sending file"); exit(1);
        }
        size -= sent_bytes;
    }
    send_reply_to_user(fd, addr, "\n");
}

void ex_bid(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID+1];
    char password[SIZE_PASSWORD+1];
    char aid[MAX_STATUS_SIZE+1];
    int value;

    sscanf(request, "%*s %s %s %s %d", uid, password, aid, &value);
    
    if (!is_auction_active(aid)) {
        send_reply_to_user(fd, addr, "RBD NOK\n");
    }
    else if (!is_logged_in(uid)) {
        send_reply_to_user(fd, addr, "RBD NLG\n");
    }
    else if (is_auct_hosted_by_user(aid, uid)) {
        send_reply_to_user(fd, addr, "RBD ILG\n");
    }
    else if (is_bid_too_small(aid, value)) {
        send_reply_to_user(fd, addr, "RBD REF\n");
    }
    else {
        create_bid(aid, uid, value);
        send_reply_to_user(fd, addr, "RBD ACC\n");
    }
}
