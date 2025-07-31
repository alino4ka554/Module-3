#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LENGTH 100
#define MAX_ARGV 50


int main(void) {
    char input[MAX_LENGTH];
    char *argv[MAX_ARGV];
    int i = 0;
    while(1) {
        printf(">>> ");
        if (!fgets(input, MAX_LENGTH, stdin))
            break;
        input[strcspn(input, "\n")] = 0;
        argv[i] = strtok(input, " ");
        while(argv[i] != NULL && i < MAX_LENGTH) {
            i++;
            argv[i] = strtok(NULL, " ");
        }
        argv[i] = NULL;
        if (argv[0] == NULL)
            continue;
        if (strcmp(argv[0], "exit") == 0)
            break;
        pid_t pid = fork();
        if(pid == -1) {
            perror("fork");
            break;
        }
        else if (pid == 0) {
            execvp(argv[0], argv);
            perror("failed");
            exit(EXIT_FAILURE);
        }
        else {
            int rv;
            wait(&rv);
        }
    }
}