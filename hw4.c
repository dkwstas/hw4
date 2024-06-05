/* C program autograder
   Author: KONSTANTINOS DRAKONTIDIS
 */

//Exposing sighandler GNU extension
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

#define BUFFER_SIZE 64
#define MAX_SCORE 100
#define WARNING_PENALTY 5
#define MEMORY_PENALTY 15

#define MAX(A, B) ((A > B) ? A : B)

typedef enum {
    SOURCE,
    ARGUMENTS,
    INPUT,
    OUTPUT,
    ERROR
} filetype_t;

typedef enum {
    COMPILATION,
    TERMINATION,
    DIFFERENCE,
    MEMORY
} penalty_t;

typedef struct {
    int heap_size;
    void **heap_array;
} heap_t;

//Alarm signal handler
static void timer_handler (int signal) {}

//Gets a memory address and checks if malloc is successful. Returns nothing.
void malloc_check (void *address) {
    if (address == NULL) {
        exit(255);
    }
}

//Gets an array of 4 integers containing penalties. Prints detailed score, returns nothing.
void print_score (int penalties[4]) {
    int total_score = 0;

    total_score = MAX(0, penalties[COMPILATION] + penalties[TERMINATION] + penalties[DIFFERENCE] +
            penalties[MEMORY]);

    printf("\nCompilation: %d\n\nTermination: %d\n\nOutput: %d\n\nMemory access: %d\n\nScore: %d\n",
            penalties[COMPILATION], penalties[TERMINATION], penalties[DIFFERENCE],
            penalties[MEMORY], total_score);
}

/*Gets the arguments filename and the name of the executable. Reads the arguments,
 *creates and returns an array of strings.
 */
char **read_arguments (char *filename, char *progname) {
    int fd, size, argc;
    char *arg_str = NULL, **argv = NULL, buffer[BUFFER_SIZE + 1], *token = NULL;

    //Opening the file with the program arguments
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return(NULL);
    }

    //Clearing the buffers
    memset(buffer, '\0', sizeof(buffer));
    arg_str = (char *)malloc(sizeof(char));
    malloc_check(arg_str);
    arg_str = (char *)memset(arg_str, '\0', 1);

    //Reading the file
    size = read(fd, buffer, BUFFER_SIZE);
    if (size == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        free(arg_str);
        return(NULL);
    }
    while (size > 0) {
        //Checking if buffer contains new line character
        if (strchr(buffer, '\n') != NULL) {
            size = strchr(buffer, '\n') - buffer;
        }
        //Adding buffer to argument string
        arg_str = (char *)realloc(arg_str, (strlen(arg_str) + size + 1) * sizeof(char));
        malloc_check(arg_str);
        arg_str = strncat(arg_str, buffer, size);
        size = read(fd, buffer, BUFFER_SIZE);
        if (size == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            free(arg_str);
            return(NULL);
        }
    }
    arg_str[strlen(arg_str)] = '\0';

    //Initialising the array of strings
    argv = (char **)malloc(sizeof(char *));
    malloc_check(arg_str);
    argv[0] = progname;

    //Converting arg_str to tokens and storing them in argv
    token = strtok(arg_str, " ");
    for (argc=1; token != NULL; argc++) {
        argv = (char **)realloc(argv, (argc + 1) * sizeof(char *));
        malloc_check(arg_str);
        argv[argc] = strdup(token);
        token = strtok(NULL, " ");
    }

    //Adding NULL as last token for execv
    argv = (char **)realloc(argv, (argc + 1) * sizeof(char *));
    malloc_check(arg_str);
    argv[argc] = NULL;

    free(arg_str);

    return(argv);
}

//Gets error file filename, checks if it contains warning and returns the amount of warnings.
int num_warnings (char *filename) {
    int fd, size, warning_count = 0;
    char buffer[BUFFER_SIZE + 1];
    const char *WARNING_STRING = "warning:";

    //Opening file
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return(-1);
    }

    //Resetting buffer
    memset(buffer, '\0', sizeof(buffer));

    //Reading file
    size = read(fd, buffer, BUFFER_SIZE);
    if (size == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return(-1);
    }
    while (size != 0) {
        //Searching the buffer for the first character of the warning string
        if (strchr(buffer, WARNING_STRING[0]) != NULL) {
            //Setting read offset to the character
            if (lseek(fd, -(size - (strchr(buffer, WARNING_STRING[0]) - buffer)), SEEK_CUR) == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                return(-1);
            }
            //Reading the block again and searching for the warning string
            size = read(fd, buffer, BUFFER_SIZE);
            if (size == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                return(-1);
            }
            //Incrementing warning count if warning is found
            if (strstr(buffer, WARNING_STRING) != NULL) {
                warning_count++;
                //Setting read offset right after the warning string
                if (lseek(fd, -size + strlen(WARNING_STRING), SEEK_CUR) == -1) {
                    fprintf(stderr, "%s\n", strerror(errno));
                    return(-1);
                }
            }
        }
        size = read(fd, buffer, BUFFER_SIZE);
        if (size == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            return(-1);
        }
    }

    close(fd);

    return(warning_count);
}

//Same as above but returns 0 if error is found
int has_error (char *filename) {
    int fd, size;
    char buffer[BUFFER_SIZE + 1];
    const char *ERROR_STRING = "error:";

    //Opening file
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return(-1);
    }

    //Resetting buffer
    memset(buffer, '\0', sizeof(buffer));

    //Reading file
    size = read(fd, buffer, BUFFER_SIZE);
    if (size == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        return(-1);
    }
    while (size != 0) {
        //Searching the buffer for the first character of the error string
        if (strchr(buffer, ERROR_STRING[0]) != NULL) {
            //Setting read offset to the character
            if (lseek(fd, -(size - (strchr(buffer, ERROR_STRING[0]) - buffer)), SEEK_CUR) == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                return(-1);
            }
            //Reading the block again and searching for the warning string
            size = read(fd, buffer, BUFFER_SIZE);
            if (size == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                return(-1);
            }
            if (strstr(buffer, ERROR_STRING) != NULL) {
                return(0);
            }
        }
        size = read(fd, buffer, BUFFER_SIZE);
        if (size == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            return(-1);
        }
    }

    close(fd);

    return(-1);
}

//Gets the filename of the source code, removes file extension and returns the new string.
char *get_progname (char *argument) {
    int i;
    char *progname = NULL;

    if (argument == NULL) {
        return(NULL);
    }

    //Searching for the position of the file extension in filename
    for (i=0; argument[i] != '\0' && argument[i] != '.'; i++);

    //Returning NULL if there is no extension
    if (argument[i] == '\0') {
        return(NULL);
    }

    progname = (char *)malloc((i + 1) * sizeof(char));
    malloc_check(progname);
    progname = (char *)memset(progname, '\0', (i + 1));
    progname = (char *)memcpy(progname, argument, i);

    return(progname);
}

//Gets a heap address array and an address. Stores the address in the array. Returns nothing.
void add_heap (heap_t *heap, void *address) {
    (heap->heap_size)++;
    heap->heap_array = realloc(heap->heap_array, (heap->heap_size) * sizeof(heap_t));
    malloc_check(heap->heap_array);
    heap->heap_array[heap->heap_size - 1] = address;
}

//Gets a heap address array and initialises it. Returns nothing.
void init_heap (heap_t **heap) {
    *heap = (heap_t *)malloc(sizeof(heap_t));
    malloc_check(*heap);
    (*heap)->heap_size = 0;
    (*heap)->heap_array = NULL;
}

//Releases all memory registered in heap address array. Returns nothing.
void free_heap (heap_t *heap) {
    int i;

    for (i=0; i < heap->heap_size; i++) {
        free(heap->heap_array[i]);
    }
    free(heap->heap_array);
    free(heap);
}

/*Gets a heap address array, the filename of the executable and an array of strings.
 *Creates and returns an array of strings that contains all necessary filenames.
 */
char **get_filenames (heap_t *heap, char *progname, char *argv[]) {
    char **filenames = NULL;
    const char ERROR_EXT[] = ".err";

    if (progname == NULL) {
        return(NULL);
    }

    filenames = (char **)malloc(5 * sizeof(char **));
    malloc_check(filenames);

    filenames[SOURCE] = argv[1];    
    filenames[ARGUMENTS] = argv[2];
    filenames[INPUT] = argv[3];
    filenames[OUTPUT] = argv[4];

    filenames[ERROR] = strdup(progname);
    filenames[ERROR] = (char *)realloc(filenames[ERROR],
            (strlen(filenames[ERROR]) + sizeof(ERROR_EXT)) * sizeof(char));
    malloc_check(filenames[ERROR]);
    add_heap(heap, filenames[ERROR]);
    filenames[ERROR] = strcat(filenames[ERROR], ERROR_EXT);

    return(filenames);
}

int main (int argc, char *argv[]) {
    int fd, p1, p2, p3, warnings, pipefd[2], p1_status = 0, p2_status = 0, p3_status = 0,
        timeout = 0, penalties[4] = { 0 };
    char *progname = NULL, *current_dir = NULL, **filenames = NULL;
    heap_t *heap = NULL;
    struct itimerval timer;
    struct sigaction action;

    //Checking if the program has received the required arguments to run
    if (argc != 6 || atoi(argv[5]) < 0) {
        fprintf(stderr, "Invalid arguments.\n Usage: ./hw4 <progname>.c <progname>.args \
                <progname>.in <progname>.out <timeout>\n");
        return(1);
    }

    //Parsing timeout from arguments
    timeout = atoi(argv[5]);

    //Initialising heap address array
    init_heap(&heap);

    //Extracting the name of the executable to be tested from arguments
    progname = get_progname(argv[1]);
    add_heap(heap, progname);
    //Creating an array of other filenames (input, output etc.)
    filenames = get_filenames(heap, progname, argv);
    add_heap(heap, filenames);

    //Creating a process to compile the code
    p1 = fork();
    if (p1 == 0) {
        //Creating a log file for compiler STDERR
        fd = open(filenames[ERROR], O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
        if (fd == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            return(255);
        }
        //Redirecting file to STDERR
        if (dup2(fd, STDERR_FILENO) == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            return(255);
        }
        if (close(fd) == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            return(255);
        }
        //Compiling code and checking if exec returned error
        if (execl("/usr/bin/gcc", "gcc", "-Wall", argv[1], "-o", progname, NULL) == -1) {
            if (close(fd) == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                return(255);
            }
            fprintf(stderr, "%s\n", strerror(errno));
            return(255);
        }
        //Checking whether the process is created
    } else if (p1 == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        free_heap(heap);
        return(255);
    }
    //Checking if the process was interrupted and quitting program if so
    if (waitpid(p1, &p1_status, 0) == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        free_heap(heap);
        return(255);
    }
    //Checking if the process returned error code and quitting program if so
    if (WEXITSTATUS(p1_status) == 255) {
        fprintf(stderr, "Received error status from p1.\n");
        free_heap(heap);
        return(255);
    }

    //Looking if the generated error file contains errors
    if (has_error(filenames[ERROR]) != 0) {
        //Counting warnings in the generated error file
        warnings = num_warnings(filenames[ERROR]);
        //Calculating penalty based on the number of warnings
        penalties[COMPILATION] = -(warnings * WARNING_PENALTY);
    } else {
        //Calculating error penalty, printing score overview and quitting
        penalties[COMPILATION] = -MAX_SCORE;
        print_score(penalties);
        free_heap(heap);
        return(0);
    }

    //Creating a pipe
    if (pipe(pipefd) == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        free_heap(heap);
        return(255);
    }

    //Creating a new process that runs the generated executable
    p2 = fork();
    if (p2 == 0) {
        //Opening the input file
        fd = open(filenames[INPUT], O_RDONLY);
        if (fd == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            return(255);
        }
        //Redirecting the input file to STDIN
        if (dup2(fd, STDIN_FILENO) == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            return(255);
        }
        //Redirecting the pipe write end to STDOUT
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            return(255);
        }
        //Attempting to close unused file descriptors
        if ((close(fd) + close(pipefd[0]) + close(pipefd[1])) < 0) {
            if ((close(STDIN_FILENO) + close(STDOUT_FILENO)) < 0) {
                fprintf(stderr, "Error closing file descriptors.\n");
                return(255);
            }
            fprintf(stderr, "Error closing file descriptors.\n");
            return(255);
        }
        //Generating path name
        current_dir = (char *)calloc(strlen(progname) + 3, sizeof(char));
        malloc_check(current_dir);
        current_dir = strcpy(current_dir, "./");
        current_dir = strcat(current_dir, progname);
        //Running the executable
        if (execv(current_dir, read_arguments(filenames[ARGUMENTS], progname)) == -1) {
            //Attempting to close open file descriptors on error
            if ((close(STDIN_FILENO) + close(STDOUT_FILENO)) < 0) {
                fprintf(stderr, "Error closing file descriptors.\n");
                free(current_dir);
                return(255);
            }
            fprintf(stderr, "%s\n", strerror(errno));
            free(current_dir);
            return(255);
        }
        //Checking if the process is not created
    } else if (p2 == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        free_heap(heap);
        return(255);
    }

    //Creating a new process to compare the output file with the output of the executable
    p3 = fork();
    if (p3 == 0) {
        //Redirecting pipe read to STDIN
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            return(255);
        }
        //Closing unused file descriptors
        if ((close(pipefd[0]) + close(pipefd[1])) < 0) {
            if (close(STDIN_FILENO) < 0) {
                fprintf(stderr, "%s\n", strerror(errno));
                return(255);
            }
            fprintf(stderr, "Error closing file descriptors.\n");
            return(255);
        }
        //Running p4diff
        if (execl("./p4diff", "p4diff", filenames[OUTPUT], NULL) == -1) {
            //Closing file descriptors on error and returning
            if (close(STDIN_FILENO) < 0) {
                fprintf(stderr, "%s\n", strerror(errno));
                return(255);
            }
            fprintf(stderr, "%s\n", strerror(errno));
            return(255);
        }
        //Checking if the process is not created
    } else if (p3 == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        free_heap(heap);
        return(255);
    }

    //Closing unused file descriptors
    if ((close(pipefd[0]) + close(pipefd[1])) < 0) {
        fprintf(stderr, "Error closing file descriptors.\n");
        kill(p2, SIGKILL);
        kill(p3, SIGKILL);
        free_heap(heap);
        return(255);
    }

    //Setting the timer
    timer.it_value.tv_sec = timeout;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    //Setting timer handler
    action.sa_handler = timer_handler;

    //Enabling signal handler and timer
    if (sigaction(SIGALRM, &action, NULL) == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        kill(p2, SIGKILL);
        kill(p3, SIGKILL);
        free_heap(heap);
        return(255);
    }
    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
        kill(p2, SIGKILL);
        kill(p3, SIGKILL);
        free_heap(heap);
        return(255);
    }

    //Checking if p2 is interrupted by signal (timer)
    if (waitpid(p2, &p2_status, 0) == -1) {
        kill(p2, SIGKILL);
        penalties[TERMINATION] = -MAX_SCORE;
    }

    //Checking if p2 indicated system call error
    if (WEXITSTATUS(p2_status) == 255) {
        kill(p3, SIGKILL);
        free_heap(heap);
        return(255);
    }

    //Checking if the executable got terminated by signal
    if (WTERMSIG(p2_status) == SIGSEGV || WTERMSIG(p2_status) == SIGABRT ||
            WTERMSIG(p2_status) == SIGBUS) {
        penalties[MEMORY] = -MEMORY_PENALTY;
    }

    //Checking if p3 is interrupted by signal
    if (waitpid(p3, &p3_status, 0) == -1) {
        kill(p3, SIGKILL);
        free_heap(heap);
        return(255);
    }

    if (WEXITSTATUS(p3_status) == 255) {
        free_heap(heap);
        return(255);
    }

    penalties[DIFFERENCE] = WEXITSTATUS(p3_status);

    print_score(penalties);

    free_heap(heap);

    return(0);
}