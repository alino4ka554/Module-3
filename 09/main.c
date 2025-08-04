#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define MAX_COUNT 50
#define MAX_LINE 1024
#define FILE_NAME "file.txt"

FILE *file;

void generation() {
    srand(time(NULL));
    file = fopen(FILE_NAME, "a");
    if (!file) {
        perror("fopen");
        exit(1);
    }
    for (int line = 0; line < 10; line++) {
        for(int i = 0; i < MAX_COUNT; i++) {
            int number = 1 + (rand() % 1000);
            fprintf(file, "%d ", number);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

int get_max(const char *line) {
    int max = 0;
    int found = 0;
    char buffer[MAX_LINE];
    strcpy(buffer, line);
    char *ptr = buffer;
    char *end;

    while (1) {
        long val = strtol(ptr, &end, 10);
        if (ptr == end) break;
        if (!found || val > max)
            max = val;
        found = 1;
        ptr = end;
    }
    return found ? max : -1;
}

int get_min(const char *line) {
    int min = 10000;
    int found = 0;
    char buffer[MAX_LINE];
    strcpy(buffer, line);
    char *ptr = buffer;
    char *end;

    while (1) {
        long val = strtol(ptr, &end, 10);
        if (ptr == end) break;
        if (!found || val < min)
            min = val;
        found = 1;
        ptr = end;
    }
    return found ? min : -1;
}

int main(void) {
    sem_t semaphore;
    sem_init(&semaphore, 1, 1);
    char line[MAX_LINE];
    int rv;
    pid_t pid;

    switch(pid = fork()) {
        case -1:
            perror("fork");
            sem_close(&semaphore);
            exit(EXIT_FAILURE);
        case 0: 
            sem_wait(&semaphore);
            file = fopen(FILE_NAME, "r+");
            if (!file) {
                perror("fopen");
                sem_post(&semaphore);
                exit(1);
            }
            fputc('*', file);
            while (fgets(line, sizeof(line), file) != NULL) {
                if (line[0] == '*') continue;
                if (strlen(line) < 2) continue;
                int max = get_max(line);
                int min = get_min(line);
                if (max == -1 || min == -1) continue;
                printf("Max: %d, min: %d\n", max, min);
                line[0] = '*';
                fputc(*line, file);
                fflush(file);
            }
            fclose(file);
            sem_post(&semaphore);
            exit(EXIT_SUCCESS);
        default: 
            sem_wait(&semaphore);
            generation();
            sem_post(&semaphore);
            wait(&rv);
            exit(EXIT_SUCCESS);
    }
}
