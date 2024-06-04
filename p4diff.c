#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <math.h>

#define BUFFER_SIZE 64

int main (int argc, char *argv[]) {
    int i, fd, stdin_size, file_size, stdin_total_size = 0, file_total_size = 0, similarities = 0, differences = 0, offset = 0;
    char *filename = NULL, stdin_buffer[BUFFER_SIZE + 1], file_buffer[BUFFER_SIZE + 1];

    if (argc != 2) {
        printf("Invalid arguments.\n");
        return(-1);
    }

    filename = argv[1];
    memset(stdin_buffer, '\0', BUFFER_SIZE + 1);
    memset(file_buffer, '\0', BUFFER_SIZE + 1);
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("Could not open file.\n");
        return(-1);
    }

    file_size = read(fd, file_buffer, BUFFER_SIZE);
    file_total_size += file_size;
    printf("FILEIO (%d:%d): %s\n", file_size, file_total_size, file_buffer);

    stdin_size = read(STDIN_FILENO, stdin_buffer, BUFFER_SIZE);
    stdin_total_size += stdin_size;
    printf("STDIN (%d:%d): %s\n", stdin_size, stdin_total_size, stdin_buffer);

    while ((file_size > 0) && (stdin_size > 0)) {
        if (file_size < stdin_size) {
            for (i=0; i < file_size; i++) {
                if (file_buffer[offset + i] == stdin_buffer[i]) {
                    similarities++;
                } else {
                    differences++;
                }
            }
            offset = file_size;
            file_size = read(fd, file_buffer, BUFFER_SIZE);
            file_total_size += file_size;
            printf("FILEIO (%d:%d): %s\n", file_size, file_total_size, file_buffer);
        } else if (file_size > stdin_size) {
            for (i=0; i < stdin_size; i++) {
                if (stdin_buffer[offset + i] == file_buffer[i]) {
                    similarities++;
                } else {
                    differences++;
                }
            }
            offset = stdin_size;
            stdin_size = read(STDIN_FILENO, stdin_buffer, BUFFER_SIZE);
            stdin_total_size += stdin_size;
            printf("STDIN (%d:%d): %s\n", stdin_size, stdin_total_size, stdin_buffer);
        } else {
            for (i=0; i < BUFFER_SIZE; i++) {
                if (stdin_buffer[i] == file_buffer[i]) {
                    similarities++;
                } else {
                    differences++;
                }
            }
            file_size = read(fd, file_buffer, BUFFER_SIZE);
            file_total_size += file_size;
            printf("FILEIO (%d:%d): %s\n", file_size, file_total_size, file_buffer);

            stdin_size = read(STDIN_FILENO, stdin_buffer, BUFFER_SIZE);
            stdin_total_size += stdin_size;
            printf("STDIN (%d:%d): %s\n", stdin_size, stdin_total_size, stdin_buffer);
        }
    }

    printf("Wrapping up...\n");

    file_size = read(fd, file_buffer, BUFFER_SIZE);
    file_total_size += file_size;
    printf("FILEIO (%d:%d): %s\n", file_size, file_total_size, file_buffer);
    while (file_size > 0) {
        file_size = read(fd, file_buffer, BUFFER_SIZE);
        file_total_size += file_size;
        printf("FILEIO (%d:%d): %s\n", file_size, file_total_size, file_buffer);
    }

    stdin_size = read(STDIN_FILENO, stdin_buffer, BUFFER_SIZE);
    stdin_total_size += stdin_size;
    printf("STDIN (%d:%d): %s\n", stdin_size, stdin_total_size, stdin_buffer);
    while (stdin_size > 0) {
        stdin_size = read(STDIN_FILENO, stdin_buffer, BUFFER_SIZE);
        stdin_total_size += stdin_size;
        printf("STDIN (%d:%d): %s\n", stdin_size, stdin_total_size, stdin_buffer);
    }

    printf("S: %d D: %d\n TF: %d TS: %d\n", similarities, differences, file_total_size, stdin_total_size);

    /*
    file_size = read(fd, file_buffer, BUFFER_SIZE);
    while (file_size != 0) {
        stdin_size = read(STDIN_FILENO, stdin_buffer, BUFFER_SIZE);
    }*/

    return(0);
}