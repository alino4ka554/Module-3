#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

bool isInt(char *str) {
    bool flag = true;
    if(*str == '\0')
        return false;
    while(*str) {
        if(!isdigit(*str))
            flag = false;
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
    }
    if(count != 1)
        flag = false;
    return flag;
}

void printProcesses(int start, int end, char* argv[]) {

}

int main(int argc, char *argv[]) {
    int mid = (argc - 1) / 2;
    pid_t pid;
    int rv;
    switch(pid = fotk()) {
        case -1:
            perror("fork");
            exit(EXIT_FAILURE);
        case 0:
            printProcesses(1, 1 + mid, argv);
            exit(EXIT_SUCCESS);
        default:
            printProcesses(1 + mid, argc, argv);
            wait(&rv);
            exit(EXIT_SUCCESS);
    }
}
