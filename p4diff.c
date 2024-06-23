/* Output difference finder for C program autograder
   Author: KONSTANTINOS DRAKONTIDIS
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define BUFFER_SIZE 64
#define MAX_SCORE 100

#define MAX(A, B) ((A > B) ? A : B)
#define MIN(A, B) ((A < B) ? A : B)

//Gets a memory address and checks if malloc is successful. Returns nothing.
void malloc_check (void *address) {
    if (address == NULL) {
        exit(255);
    }
}

/*Gets a file descriptor and a buffer, fills the whole buffer with data (if possible) from fd. 
 *Returns the amount of data written in the buffer, 255 on error.
 */
int update_buffer (int fd, char buffer[BUFFER_SIZE + 1]) {
    int size = 0, total_size = 0;
    char *temp_buffer = NULL;

    //Resetting the buffer
    memset(buffer, '\0', BUFFER_SIZE + 1);

    //Reading from fd
    size = read(fd, buffer, BUFFER_SIZE);
    //Exception handling
    if (size == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return(255);
    }
    total_size += size;
    //Reading until the buffer is full
    while (size != 0 && total_size < BUFFER_SIZE) {
        //Creating a temp buffer to store new data
        temp_buffer = (char *)realloc(temp_buffer, (BUFFER_SIZE - total_size + 1) * sizeof(char));
        malloc_check(temp_buffer);
        temp_buffer = (char *)memset(temp_buffer, '\0',
                (BUFFER_SIZE - total_size + 1) * sizeof(char));
        size = read(fd, temp_buffer, BUFFER_SIZE - total_size);
        if (size == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            return(255);
        }
        total_size += size;
        //Copying data over to main buffer from temp buffer
        strcat(buffer, temp_buffer);
    }

    free(temp_buffer);

    return(total_size);
}

int main (int argc, char *argv[]) {
    int i, fd, similarities = 0, differences = 0, stdin_size = 0, file_size = 0,
        total_stdin_size = 0, total_file_size = 0, percentage = 0;
    char stdin_buffer[BUFFER_SIZE + 1], file_buffer[BUFFER_SIZE + 1];

    //Checking if the program has received the required arguments to run
    if (argc != 2) {
        fprintf(stderr, "Invalid arguments.\n");
        return(255);
    }

    //Opening file
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return(255);
    }

    //Attempting to read data from file descriptors
    stdin_size = update_buffer(STDIN_FILENO, stdin_buffer);
    file_size = update_buffer(fd, file_buffer);
    total_stdin_size += stdin_size;
    total_file_size += file_size;
    //Comparing the two buffers
    while (stdin_size != 0 || file_size != 0) {
        for (i=0; i < MIN(stdin_size, file_size); i++) {
            if (stdin_buffer[i] == file_buffer[i]) {
                similarities++;
            } else {
                differences++;
            }
        }

        stdin_size = update_buffer(STDIN_FILENO, stdin_buffer);
        file_size = update_buffer(fd, file_buffer);
        total_stdin_size += stdin_size;
        total_file_size += file_size;
    }

    if (close(fd) == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return(255);
    }

    //Calculating and returning the score

    if (total_file_size == 0 && total_stdin_size == 0) {
        return(MAX_SCORE);
    }

    percentage = (similarities * 100) / MAX(total_file_size, total_stdin_size);

    return(percentage);
}