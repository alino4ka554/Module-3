#include "message.h"

int msqid;
int clients[100];  // макс. клиентов
int client_count = 0;

int is_shutdown(const char *msg) {
    return strcmp(msg, "shutdown") == 0;
}

int is_known_client(int id) {
    for (int i = 0; i < client_count; i++)
        if (clients[i] == id)
            return 1;
    return 0;
}

void add_client(int id) {
    if (!is_known_client(id)) {
        clients[client_count++] = id;
        printf("Server: client connected %d\n", id);
    }
}

void remove_client(int id) {
    for (int i = 0; i < client_count; i++) {
        if (clients[i] == id) {
            clients[i] = clients[--client_count];
            printf("Server: client %d disconnected\n", id);
            break;
        }
    }
}

int is_connect(const char *msg) {
    return strcmp(msg, "CONNECT") == 0;
}

int main(void) {
    key_t key = ftok("server.c", 1);
    if(key == -1) {
        printf("ERROR");
        exit(0);
    }
    msqid = msgget(key, IPC_CREAT | 0666);
    message msg;
    while(1) {
        if(msgrcv(msqid, &msg, sizeof(msg) - sizeof(long), SERVER_TYPE, 0) == -1) {
            perror("msgrcv");
            break;
        }
        if (is_connect(msg.text)) {
            add_client(msg.sender);
            continue; 
        }
        if (is_shutdown(msg.text)) {
            remove_client(msg.sender);
            continue;
        }
        for (int i = 0; i < client_count; i++) {
            if (clients[i] == msg.sender) continue;
            msg.mtype = clients[i];
            if (msgsnd(msqid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
                perror("msgsnd");
            }
        }
    }
    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}