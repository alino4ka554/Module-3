#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include <stdlib.h>
#include "client.h"

int main(void) {
    struct mq_attr queue_attr;
    queue_attr.mq_flags = 0;
    queue_attr.mq_maxmsg = 10;
    queue_attr.mq_msgsize = MAX_SIZE;
    queue_attr.mq_curmsgs = 0;
    mqd_t in_q = mq_open(QUEUE_A, O_RDONLY | O_CREAT, 0666, &queue_attr);
    mqd_t out_q = mq_open(QUEUE_B, O_WRONLY | O_CREAT, 0666, &queue_attr);
    if (out_q == -1 || in_q == -1) {
        perror("mq_open");
        exit(1);
    }
    char buffer[MAX_SIZE];
    int prio;
    while(1) {
        printf("A: ");
        fgets(buffer, MAX_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;
        prio = (strcmp(buffer, "exit") == 0) ? STOP_PRIORITY : 1;
        if (mq_send(out_q, buffer, strlen(buffer) + 1, prio) == -1) {
            perror("mq_send");
            break;
        }
        if (prio == STOP_PRIORITY)
            break;
        if (mq_receive(in_q, buffer, MAX_SIZE, &prio) == -1){
            perror("cannot receive");
            break;
        }
        else if (prio == STOP_PRIORITY) break;
        printf("From B: %s\n", buffer);
    }
    mq_close(out_q);
    mq_close(in_q);
    mq_unlink(QUEUE_A);
    mq_unlink(QUEUE_B);
    return 0;
}