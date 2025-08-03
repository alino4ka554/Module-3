#include "message.h"

int msqid;
int client_id;

void *receive_messages(void *arg) {
    message msg;
    while (1) {
        if (msgrcv(msqid, &msg, sizeof(msg) - sizeof(long), client_id, 0) != -1) {
            printf("\n[Client %d] From %d: %s\n>>> ", client_id, msg.sender, msg.text);
            fflush(stdout);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Please, enter client priority");
    }
    key_t key = ftok("server.c", 1);
    if(key == -1) {
        printf("ERROR");
        exit(0);
    }
    client_id = atoi(argv[1]);
    msqid = msgget(key, IPC_CREAT | 0666);
    pthread_t thread;
    pthread_create(&thread, NULL, receive_messages, NULL);
    message msg;
    msg.mtype = SERVER_TYPE;
    char input[256];
    printf("Client %d connected. Enter message:\n", client_id);
    strcpy(msg.text, "CONNECT");
    msg.mtype = SERVER_TYPE; 
    msg.sender = client_id; 
    if (msgsnd(msqid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }
    while (1) {
        printf(">>> ");
        if (!fgets(input, 256, stdin)) break;
        input[strcspn(input, "\n")] = 0;
        strncpy(msg.text, input, 256); 
        msg.sender = client_id;        
        if (msgsnd(msqid, &msg, sizeof(msg) - sizeof(long), 0) == -1) {
            perror("msgsnd");
            break;
        }
        if (strcmp(input, "shutdown") == 0) 
            break;
    }
    return 0;
}