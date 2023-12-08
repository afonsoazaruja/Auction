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

    if (is_logged_in(path_user_dir, uid)) {
        send_reply_to_user(fd, addr, "user already logged in\n");
        return;
    }
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
    char *login_file_name = get_file_name(uid, path_user_dir, "_login.txt");

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
    char *login_file_name = get_file_name(uid, path_user_dir, "_login.txt");
    char *pass_file_name = get_file_name(uid, path_user_dir, "_pass.txt");

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
    send_reply_to_user(fd, addr, "TEST\n");
}
void ex_mybids(int fd, struct sockaddr_in addr, char *request) {
    // To do
    send_reply_to_user(fd, addr, "TEST\n");
}
void ex_list(int fd, struct sockaddr_in addr, char *request) {
    // To do
    send_reply_to_user(fd, addr, "TEST\n");
}
void ex_show_record(int fd, struct sockaddr_in addr, char *request) {
    // To do
    send_reply_to_user(fd, addr, "TEST\n");
}