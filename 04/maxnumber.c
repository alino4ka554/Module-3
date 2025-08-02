#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int max = 0;
    for(int i = 0; i < argc; i++) {
        if(i == 0) max = atoi(argv[i]);
        else if (max < atoi(argv[i]))
            max = atoi(argv[i]);
    }
    printf("MAX: %d\n", max);
    return 0;
}