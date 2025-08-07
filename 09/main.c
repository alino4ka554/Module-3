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
#define SEMAPHORE_NAME "/my_semaphore"

void generation() {
    srand(time(NULL));
    FILE *file = fopen(FILE_NAME, "a"); 
    if (!file) {
        perror("fopen");
        exit(1);
    }

    for (int line = 0; line < 10; line++) {
        for (int i = 0; i < MAX_COUNT; i++) {
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
        if (!found || val > max) max = val;
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
        if (!found || val < min) min = val;
        found = 1;
        ptr = end;
    }
    return found ? min : -1;
}

int main(void) {
    sem_t *semaphore = sem_open(SEMAPHORE_NAME, O_CREAT, 0666, 1);
    if (semaphore == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        sem_close(semaphore);
        sem_unlink(SEMAPHORE_NAME);
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        sem_wait(semaphore);
        FILE *file = fopen(FILE_NAME, "r+");
        if (!file) {
            perror("fopen");
            sem_post(semaphore);
            exit(1);
        }

        char line[MAX_LINE];
        long offset;
        while ((offset = ftell(file)), fgets(line, sizeof(line), file)) {
            if (line[0] == '*') continue; 

            int max = get_max(line);
            int min = get_min(line);
            if (max == -1 || min == -1) continue;

            printf("Max: %d, min: %d\n", max, min);
            fseek(file, offset, SEEK_SET); 
            fputc('*', file);
            fflush(file);
            fseek(file, offset + strlen(line), SEEK_SET); 
        }

        fclose(file);
        sem_post(semaphore);
        sem_close(semaphore);
        exit(EXIT_SUCCESS);
    } else {
        sem_wait(semaphore);
        generation();
        sem_post(semaphore);

        wait(NULL);

        sem_close(semaphore);
        sem_unlink(SEMAPHORE_NAME);
    }

    return 0;
}
