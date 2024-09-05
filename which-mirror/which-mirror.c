#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <unistd.h>

#define MAX_USERS 100
#define MAX_LENGTH 32

typedef enum { TYPE_A, TYPE_B } UserType;

struct UserA {
    unsigned long long uid;
    unsigned long long phone;
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
} __attribute__((packed));

struct UserB {
    long long uid;
    long long phone;
    char password[MAX_LENGTH];
    char username[MAX_LENGTH];
} __attribute__((packed));

typedef struct {
    UserType type;
    union {
        struct UserA a;
        struct UserB b;
    } user;
} __attribute__((packed)) User;

User *users[MAX_USERS];
int user_count = 0;
long long next_uid = 1;

void init() {
    setvbuf(stdin, 0, 2, 0);
    setvbuf(stdout, 0, 2, 0);
}

void register_user(UserType type) {
    if (user_count >= MAX_USERS) {
        printf("Maximum number of users reached.\n");
        return;
    }
    char username[33];
    char password[33];
    long long phone;

    username[32] = '\0';
    password[32] = '\0';

    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);
    printf("Enter phone number: ");
    scanf("%lld", &phone);

    User *new_user = (User *)malloc(sizeof(User));
    new_user->type = type;
    int username_len =
        MAX_LENGTH - 1 > strlen(username) ? strlen(username) : MAX_LENGTH - 1;
    int password_len =
        MAX_LENGTH - 1 > strlen(password) ? strlen(password) : MAX_LENGTH - 1;

    for (int i = 0; i < user_count; i++) {
        char *user_username = (users[i]->type == TYPE_A)
                                  ? users[i]->user.a.username
                                  : users[i]->user.b.username;
        if (strcmp(user_username, username) == 0) {
            printf("Username already exists.\n");
            return;
        }
    }

    if (type == TYPE_A) {
        new_user->user.a.uid = next_uid++;
        new_user->user.a.phone = phone;
        strncpy(new_user->user.a.username, username, strlen(username));
        new_user->user.a.username[username_len] = '\0';
        strncpy(new_user->user.a.password, password, strlen(password));
        new_user->user.a.password[password_len] = '\0';
    } else {
        new_user->user.b.uid = next_uid++;
        new_user->user.b.phone = phone;
        strncpy(new_user->user.b.username, username, strlen(username));
        new_user->user.b.username[username_len] = '\0';
        strncpy(new_user->user.b.password, password, strlen(password));
        new_user->user.b.password[password_len] = '\0';
    }

    users[user_count++] = new_user;
    printf("User registered successfully with UID: %lld\n", next_uid - 1);
}

void register_admin() {
    User *admin_user = (User *)malloc(sizeof(User));
    admin_user->type = TYPE_A;
    admin_user->user.a.uid = next_uid++;
    strcpy(admin_user->user.a.username, "admin");
    strcpy(admin_user->user.a.password, "super_secret_password");
    getrandom((void *)&admin_user->user.a.phone, 5, 0);
    users[user_count++] = admin_user;
}

void delete_user(char *username) {
    if (strcmp(username, "admin") == 0) {
        printf("Cannot delete admin user.\n");
        return;
    }
    for (int i = user_count - 1; i >= 0; i--) {
        char *user_username = (users[i]->type == TYPE_A)
                                  ? users[i]->user.a.username
                                  : users[i]->user.b.username;
        if (strcmp(user_username, username) == 0) {
            free(users[i]);
            users[i] = users[--user_count];
            printf("User deleted successfully.\n");
            return;
        }
    }
    printf("User not found.\n");
}

void get_flag(char *username) {
    if (strcmp(username, "admin") == 0) {
        printf("Congratulations! Here's your flag: "
               "CSeC{1_4m_50m3th1n6_0f_4n_4dm1n_my5e1f}\n");
    } else {
        printf("Access denied. Only the admin can get the flag.\n");
    }
}

void post_login_menu(char *username) {
    char choice[10];

    while (1) {
        printf(
            "\n1. Delete Account\n2. Get Flag\n3. Logout\nEnter your choice: ");
        scanf("%s", choice);

        switch (choice[0]) {
        case '1':
            delete_user(username);
            return;
        case '2':
            get_flag(username);
            break;
        case '3':
            printf("Logged out successfully.\n");
            return;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }
}

void login() {
    char username[33];
    char password[33];

    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    username[32] = '\0';
    password[32] = '\0';

    for (int i = user_count - 1; i >= 0; i--) {
        char *user_username = (users[i]->type == TYPE_A)
                                  ? users[i]->user.a.username
                                  : users[i]->user.b.username;
        char *user_password = (users[i]->type == TYPE_A)
                                  ? users[i]->user.a.password
                                  : users[i]->user.b.password;

        if (strcmp(user_username, username) == 0 &&
            strcmp(user_password, password) == 0) {
            printf("Login successful. Welcome, %s (UID: %lld)!\n", username,
                   (users[i]->type == TYPE_A) ? users[i]->user.a.uid
                                              : users[i]->user.b.uid);
            post_login_menu(username);
            return;
        }
    }
    printf("Invalid username or password.\n");
}

void view_users() {
    printf("Registered users:\n");
    for (int i = 0; i < user_count; i++) {
        printf("UID: %lld, Type: %s, Username: %s\n",
               (users[i]->type == TYPE_A) ? users[i]->user.a.uid
                                          : users[i]->user.b.uid,
               (users[i]->type == TYPE_A) ? "A" : "B",
               (users[i]->type == TYPE_A) ? users[i]->user.a.username
                                          : users[i]->user.b.username);
        printf("Phone: %lld\n", (users[i]->type == TYPE_A)
                                    ? users[i]->user.a.phone
                                    : users[i]->user.b.phone);
    }
}

int main() {
    init();
    register_admin();

    char choice[10];
    UserType type;

    while (1) {
        printf("\n1. Register at Mirror 1\n2. Register at Mirror 2\n3. "
               "Login\n4. View "
               "Users\n5. Exit\nEnter your choice: ");
        scanf("%s", choice);

        switch (choice[0]) {
        case '1':
        case '2':
            type = (choice[0] == '1') ? TYPE_A : TYPE_B;
            register_user(type);
            break;
        case '3':
            login();
            break;
        case '4':
            view_users();
            break;
        case '5':
            printf("Goodbye!\n");
            return 0;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    }
    return 0;
}
