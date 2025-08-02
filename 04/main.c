#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_LENGTH 100
#define MAX_ARGV 50

void run_pipeline(char *cmd) {
    char *cmds[100];
    cmds[0] = strtok(cmd, "|");
    int i = 0;
    while(cmds[i] != NULL && i < MAX_LENGTH) {
        i++;
        cmds[i] = strtok(NULL, "|");
    }
    int prev_pipe_fd[2] = {-1, -1};
    int cmd_count = i; 
    for (int i = 0; i < cmd_count; i++) { 
        int pipe_fd[2];
        if (i != cmd_count - 1) pipe(pipe_fd); 
        char *infile = NULL, *outfile = NULL;
        char *cmd_line = strdup(cmds[i]);
        char *redirect_in = strchr(cmd_line, '<');
        char *redirect_out = strchr(cmd_line, '>');
        if (redirect_in) {
            *redirect_in = '\0';
            infile = strtok(redirect_in + 1, " ");
        }
        if (redirect_out) {
            *redirect_out = '\0';
            outfile = strtok(redirect_out + 1, " ");
        }
        pid_t pid = fork();
        if (pid == 0) {
            if (i > 0) {
                dup2(prev_pipe_fd[0], STDIN_FILENO);
                close(prev_pipe_fd[0]);
                close(prev_pipe_fd[1]);
            }
            if (i != cmd_count - 1) {
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[0]);
                close(pipe_fd[1]);
            }
            if (infile) {
                int fd = open(infile, O_RDONLY);
                if (fd == -1) { perror("open infile"); exit(1); }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }
            if (outfile) {
                int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (fd == -1) { perror("open outfile"); exit(1); }
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }
            char *argv[MAX_ARGV];
            int argc = 0;
            argv[argc] = strtok(cmd_line, " ");
            while (argv[argc] != NULL && argc < MAX_ARGV - 1) {
                argc++;
                argv[argc] = strtok(NULL, " ");
            }
            argv[argc] = NULL;
            execvp(argv[0], argv);
            perror("exec failed");
            exit(EXIT_FAILURE);
        } else if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if (i > 0) {
            close(prev_pipe_fd[0]);
            close(prev_pipe_fd[1]);
        }
        if (i != cmd_count - 1) {
            prev_pipe_fd[0] = pipe_fd[0];
            prev_pipe_fd[1] = pipe_fd[1];
        }
        free(cmd_line);
    }
    for (int i = 0; i < cmd_count; i++) wait(NULL);
}


int main(void) {
    char input[MAX_LENGTH];
    int i = 0;
    while(1) {
        printf(">>> ");
        if (!fgets(input, MAX_LENGTH, stdin))
            break;
        input[strcspn(input, "\n")] = 0;
        if (strcmp(input, "exit") == 0)
        break;
        run_pipeline(input);
    }
}