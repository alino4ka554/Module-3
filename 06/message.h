#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define SERVER_TYPE 10

typedef struct message {
    long mtype;
    char text[100];
    int sender;
} message;

