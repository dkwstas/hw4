/* Output difference finder for C program autograder
   Author: KONSTANTINOS DRAKONTIDIS
 */

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
    int i, fd, stdin_size, file_size, stdin_total_size = 0, file_total_size = 0,
        similarities = 0, differences = 0, stdin_offset = 0, file_offset = 0, percentage = 0;
    char *filename = NULL, stdin_buffer[BUFFER_SIZE + 1], file_buffer[BUFFER_SIZE + 1];

    //Checking if the program has received the required arguments to run
    if (argc != 2) {
        printf("Invalid arguments.\n");
        return(-1);
    }

    //Clearing file buffer and stdin buffer
    filename = argv[1];
    memset(stdin_buffer, '\0', BUFFER_SIZE + 1);
    memset(file_buffer, '\0', BUFFER_SIZE + 1);

    //Opening file
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return(255);
    }

    //Reading a block of BUFFER_SIZE from each file descriptor
    file_size = read(fd, file_buffer, BUFFER_SIZE);
    if (file_size == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return(255);
    }
    file_total_size += file_size;

    stdin_size = read(STDIN_FILENO, stdin_buffer, BUFFER_SIZE);
    if (stdin_size == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return(255);
    }
    stdin_total_size += stdin_size;
    //Read until one file descriptor stops providing data
    while (file_size > 0 && stdin_size > 0) {
        /*Checking which buffer contains less "new" data and comparing its bytes with
         *the other one after its last offset
         */
        for (i=0; i < MIN(file_size - file_offset, stdin_size - stdin_offset); i++) {
            if (stdin_buffer[stdin_offset + i] == file_buffer[file_offset + i]) {
                similarities++;
            } else {
                differences++;
            }
        }

        //If stdin had less "new" data, read from stdin and set new offset to file
        if ((file_size - file_offset) > (stdin_size - stdin_offset)) {
            file_offset += i;
            stdin_offset = 0;

            stdin_size = read(STDIN_FILENO, stdin_buffer, BUFFER_SIZE);
            if (stdin_size == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                return(255);
            }
            stdin_total_size += stdin_size;
            //If file had less "new" data, read from file and set new offset to stdin
        } else if ((stdin_size - stdin_offset) > (file_size - file_offset)) {
            file_offset = 0;
            stdin_offset += i;

            file_size = read(fd, file_buffer, BUFFER_SIZE);
            if (file_size == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                return(255);
            }
            file_total_size += file_size;
            //If both had the same amount of data, read from file and stdin
        } else {
            file_offset = 0;
            stdin_offset = 0;

            file_size = read(fd, file_buffer, BUFFER_SIZE);
            if (file_size == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                return(255);
            }
            file_total_size += file_size;

            stdin_size = read(STDIN_FILENO, stdin_buffer, BUFFER_SIZE);
            if (stdin_size == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                return(255);
            }
            stdin_total_size += stdin_size;
        }
    }

    //Attempting to read data that was left behind (outputs were not equal size)
    file_size = read(fd, file_buffer, BUFFER_SIZE);
    if (file_size == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return(255);
    }
    file_total_size += file_size;
    while (file_size > 0) {
        file_size = read(fd, file_buffer, BUFFER_SIZE);
        if (file_size == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            return(255);
        }
        file_total_size += file_size;
    }
    stdin_size = read(STDIN_FILENO, stdin_buffer, BUFFER_SIZE);
    if (stdin_size == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return(255);
    }
    stdin_total_size += stdin_size;
    while (stdin_size > 0) {
        stdin_size = read(STDIN_FILENO, stdin_buffer, BUFFER_SIZE);
        if (stdin_size == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            return(255);
        }
        stdin_total_size += stdin_size;
    }

    if (file_total_size == 0 && stdin_total_size == 0) {
        return(MAX_SCORE);
    }

    percentage = (similarities * 100) / MAX(file_total_size, stdin_total_size);

    return(percentage);
}