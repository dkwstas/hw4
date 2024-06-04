#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main (int argc, char *argv[]) {
    int i, numbers[2] = {0};

    //printf("DEBUG:\n Arguments (%d):", argc - 1);
    //for (i=1; i < argc; i++) {
    //    printf(" %s", argv[i]);
    //}
    //printf("\n");

    if ((argc != 3) || (strcmp(argv[1], "add") != 0) || (strcmp(argv[2], "sub") != 0)) {
        printf("Invalid arguments!\n");
        return(-1);
    }

    scanf("%d %d", &numbers[0], &numbers[1]);

    //sleep(5);

    printf("Result: %d\n", numbers[0] + numbers[1]);

    return(0);
}