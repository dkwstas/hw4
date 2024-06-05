#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_BUFFER 1024

int get_rand (int upper, int lower) {
    return((rand() % (upper - lower + 1)) + lower);
}

int main (void) {
    int fd, r, size, total = 0;
    char buffer[MAX_BUFFER + 1];

    srand(time(NULL));
    memset(buffer, '\0', MAX_BUFFER + 1);

    fd = open("loremsmall", O_RDONLY);
    //dup2(fd, STDOUT_FILENO);

    r = get_rand(MAX_BUFFER, 1);
    size = read(fd, buffer, r);
    write(STDOUT_FILENO, buffer, size);
    total += size;
    while (size > 0) {
        r = get_rand(MAX_BUFFER, 1);
        size = read(fd, buffer, r);
        write(STDOUT_FILENO, buffer, size);
        total += size;
        //printf("\n%d", r);
        usleep(get_rand(100000, 10000));
    }

    //printf("\n\n%d\n\n", total);

    return(0);
}