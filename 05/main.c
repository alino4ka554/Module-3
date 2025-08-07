#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>

FILE *file;
int countSIGINT = 0;

void processing(int signal) {
    if(signal == SIGINT || signal == SIGQUIT) {
        char *message = (signal == SIGINT) ? "Received SIGINT\n"  : "Received SIGOUT\n";
        fprintf(file, "%s", message);
        fflush(file);
        if(signal == SIGINT) {
            countSIGINT++;
            if(countSIGINT == 3) {
                printf("PROCESS KILLED\n");
                fclose(file);
                exit(0);
            }
        }
    }
}

int main() {
    file = fopen("counter.txt", "w");
    if (file == NULL) {
        perror("Ошибка открытия файла");
        return 1;
    }
    signal(SIGINT, processing);
    signal(SIGQUIT, processing);
    int counter = 1;
    while (1) {
        fprintf(file, "%d\n", counter);
        fflush(file); 
        counter++;
        sleep(1);
    }

    fclose(file);
    return 0;
}