#include "executes.h"
#include "aux_executes.h"

void ex_login(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID + 1];
    char password[SIZE_PASSWORD + 1];
    FILE *pass_file;
    FILE *uid_file;
    char path_user_dir[100];

    sscanf(request, "%*s %s %s", uid, password);
    build_path_user_dir(path_user_dir, uid);

    if (is_logged_in(path_user_dir, uid)) {
        send_reply_to_user(fd, addr, "user already logged in\n");
        return;
    }
    if (is_registered(path_user_dir, uid)) {
        if (is_correct_password(password, path_user_dir, uid)) {
            create_login_file(uid, path_user_dir);
            send_reply_to_user(fd, addr, "RLI OK\n");
        } else {
            send_reply_to_user(fd, addr, "RLI NOK\n");
        }
    } else {
        if (!directoryExists(path_user_dir)) {
            if (mkdir(path_user_dir, 0777) != 0) {
                perror("Error creating directory"); exit(1);
            }
        }
        create_login_file(uid, path_user_dir);
        create_pass_file(uid, password, path_user_dir);
        send_reply_to_user(fd, addr, "RLI REG\n");
    }
}

void ex_logout(int fd, struct sockaddr_in addr, char *request) {
    char uid[SIZE_UID + 1];
    char path_user_dir[100];

    sscanf(request, "%*s %s", uid);
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
    char path_user_dir[100];

    sscanf(request, "%*s %s", uid);
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