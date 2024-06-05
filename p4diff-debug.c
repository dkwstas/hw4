#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <math.h>

#define BUFFER_SIZE 64
#define MAX_SCORE 100

#define MAX(A, B) ((A > B) ? A : B)
#define MIN(A, B) ((A < B) ? A : B)

int main (int argc, char *argv[]) {
    int i, fd, stdin_size, file_size, stdin_total_size = 0, file_total_size = 0, similarities = 0, differences = 0, stdin_offset = 0, file_offset = 0, percentage = 0;
    char *filename = NULL, stdin_buffer[BUFFER_SIZE + 1], file_buffer[BUFFER_SIZE + 1];

    //Checking if the program has received the required arguments to run
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
    //printf("FILEIO (%d:%d): %s\n", file_size, file_total_size, file_buffer);

    stdin_size = read(STDIN_FILENO, stdin_buffer, BUFFER_SIZE);
    stdin_total_size += stdin_size;
    //printf("STDIN (%d:%d): %s\n", stdin_size, stdin_total_size, stdin_buffer);

    while (file_size > 0 && stdin_size > 0) {
        //printf("\n=====================================\n");
        for (i=0; i < MIN(file_size - file_offset, stdin_size - stdin_offset); i++) {
            if (stdin_buffer[stdin_offset + i] == file_buffer[file_offset + i]) {
                //printf("%c=%c", stdin_buffer[stdin_offset + i], file_buffer[file_offset + i]);
                similarities++;
            } else {
                //printf("%c!%c", stdin_buffer[stdin_offset + i], file_buffer[file_offset + i]);
                differences++;
            }
        }
        //printf("\n=====================================\n");

        if ((file_size - file_offset) > (stdin_size - stdin_offset)) {
            file_offset += i;
            stdin_offset = 0;

            stdin_size = read(STDIN_FILENO, stdin_buffer, BUFFER_SIZE);
            stdin_total_size += stdin_size;
            //printf("STDIN (%d:%d): %s\n", stdin_size, stdin_total_size, stdin_buffer);
        } else if ((stdin_size - stdin_offset) > (file_size - file_offset)) {
            file_offset = 0;
            stdin_offset += i;

            file_size = read(fd, file_buffer, BUFFER_SIZE);
            file_total_size += file_size;
            //printf("FILEIO (%d:%d): %s\n", file_size, file_total_size, file_buffer);
        } else {
            file_offset = 0;
            stdin_offset = 0;

            file_size = read(fd, file_buffer, BUFFER_SIZE);
            file_total_size += file_size;
            //printf("FILEIO (%d:%d): %s\n", file_size, file_total_size, file_buffer);

            stdin_size = read(STDIN_FILENO, stdin_buffer, BUFFER_SIZE);
            stdin_total_size += stdin_size;
            //printf("STDIN (%d:%d): %s\n", stdin_size, stdin_total_size, stdin_buffer);
        }
    }


    //printf("Wrapping up...\n");

    file_size = read(fd, file_buffer, BUFFER_SIZE);
    file_total_size += file_size;
    //printf("FILEIO (%d:%d): %s\n", file_size, file_total_size, file_buffer);
    while (file_size > 0) {
        file_size = read(fd, file_buffer, BUFFER_SIZE);
        file_total_size += file_size;
        //printf("FILEIO (%d:%d): %s\n", file_size, file_total_size, file_buffer);
    }

    stdin_size = read(STDIN_FILENO, stdin_buffer, BUFFER_SIZE);
    stdin_total_size += stdin_size;
    //printf("STDIN (%d:%d): %s\n", stdin_size, stdin_total_size, stdin_buffer);
    while (stdin_size > 0) {
        stdin_size = read(STDIN_FILENO, stdin_buffer, BUFFER_SIZE);
        stdin_total_size += stdin_size;
        //printf("STDIN (%d:%d): %s\n", stdin_size, stdin_total_size, stdin_buffer);
    }

    //printf("S: %d D: %d\n TF: %d TS: %d\n", similarities, differences, file_total_size, stdin_total_size);

    if (file_total_size == 0 && stdin_total_size == 0) {
        return(MAX_SCORE);
    }

    percentage = (similarities * 100) / MAX(file_total_size, stdin_total_size);

    return(percentage);
}