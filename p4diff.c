#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 64

int main (int argc, char *argv[]) {
    int fd, stdin_size, file_buffer;
    char *filename = NULL, stdin_buffer[BUFFER_SIZE + 1], file_buffer[BUFFER_SIZE + 1];

    if (argc != 2) {
        printf("Invalid arguments.\n");
        return(-1);
    }

    filename = argv[1];
    memset(stdin_buffer, '\0', BUFFER_SIZE + 1);
    memset(file_buffer, '\0', BUFFER_SIZE + 1);

    stdin_size = read(STDIN_FILENO, stdin_buffer, BUFFER_SIZE);
    fd = open(filename, O_RDONLY);
    while (stdin_size != 0) {
        file_buffer = read(fd, file_buffer, BUFFER_SIZE);

    }
    



    return(0);
}