#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/wait.h>

bool isInt(char *str) {
    bool flag = true;
    if(*str == '\0')
        return false;
    while(*str) {
        if(!isdigit(*str))
            flag = false;
        str++;
    }
    return flag;
}

bool isFLoat(char *str) {
    bool flag = true;
    int count = 0;
    if(*str == '\0')
        return false;
    while(*str) {
        if(*str == '.')
            count++;
        else if (!isdigit(*str)) 
            flag = false;
        str++;
    }
    if(count != 1)
        flag = false;
    return flag;
}

void printProcesses(int start, int end, char* argv[], char *str) {
    for (int i = start; i < end; i++) {
        if(isInt(argv[i])) {
            int num = atoi(argv[i]);
            printf("%s: PID - %d: %d\n", str, getpid(), num * 2);
        }
        else if (isFLoat(argv[i])) {
            float num = atof(argv[i]);
            printf("%s: PID - %d: %f\n", str, getpid(), num * 2);
        }
        else
            printf("%s: PID - %d: %s\n", str, getpid(), argv[i]);
    }
}

int main(int argc, char *argv[]) {
    int mid = (argc - 1) / 2;
    pid_t pid;
    int rv;
    switch(pid = fork()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            printProcesses(1, 1 + mid, argv, "child");
            exit(EXIT_SUCCESS);
        default:
            printProcesses(1 + mid, argc, argv, "parent");
            wait(&rv);
            exit(EXIT_SUCCESS);
    }
}
