#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFER_SIZE 64

typedef enum {
    SOURCE,
    ARGUMENTS,
    INPUT,
    OUTPUT,
    ERROR
} filetype_t;

typedef struct {
    int heap_size;
    void **heap_array;
} heap_t;

char **read_arguments (char *filename, char *progname) {
    int fd, size, argc;
    char *arg_str = NULL, **argv = NULL, buffer[BUFFER_SIZE + 1], *token = NULL;

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("Could not open file.\n");
        return(NULL);
    }

    memset(buffer, '\0', sizeof(buffer));

    arg_str = (char *)malloc(sizeof(char));
    arg_str = (char *)memset(arg_str, '\0', 1);

    size = read(fd, buffer, BUFFER_SIZE);
    while (size != 0) {
        if (strchr(buffer, '\n') != NULL) {
            size = strchr(buffer, '\n') - buffer;
        }
        arg_str = (char *)realloc(arg_str, (strlen(arg_str) + size + 1) * sizeof(char));
        arg_str = strncat(arg_str, buffer, size);
        size = read(fd, buffer, BUFFER_SIZE);
    }
    arg_str[strlen(arg_str)] = '\0';

    argv = (char **)malloc(sizeof(char *));
    argv[0] = progname;

    token = strtok(arg_str, " ");
    for (argc=1; token != NULL; argc++) {
        argv = (char **)realloc(argv, (argc + 1) * sizeof(char *));
        argv[argc] = strdup(token);
        token = strtok(NULL, " ");
    }

    argv = (char **)realloc(argv, (argc + 1) * sizeof(char *));
    argv[argc] = NULL;
    
    return(argv);
}

int num_warnings (char *filename) {
    int fd, size, warning_count = 0;
    char buffer[BUFFER_SIZE + 1];
    const char *WARNING_STRING = "warning:";

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("Could not open file.\n");
        return(-1);
    }

    memset(buffer, '\0', sizeof(buffer));

    size = read(fd, buffer, BUFFER_SIZE);
    while (size != 0) {
        if (strstr(buffer, WARNING_STRING) != NULL) {
            warning_count++;
        }
        size = read(fd, buffer, BUFFER_SIZE);
    }

    close(fd);

    return(warning_count);
}

int has_error (char *filename) {
    int fd, size;
    char buffer[BUFFER_SIZE + 1];
    const char *ERROR_STRING = "error:";

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("Could not open file.\n");
        return(-1);
    }

    memset(buffer, '\0', sizeof(buffer));

    size = read(fd, buffer, BUFFER_SIZE);
    while (size != 0) {
        if (strstr(buffer, ERROR_STRING) != NULL) {
            return(0);
        }
        size = read(fd, buffer, BUFFER_SIZE);
    }

    close(fd);

    return(-1);
}

char *get_progname (char *argument) {
    int i;
    char *progname = NULL;

    for (i=0; argument[i] != '\0' && argument[i] != '.'; i++);

    if (argument[i] == '\0') {
        return(NULL);
    }

    progname = (char *)malloc((i + 1) * sizeof(char));
    progname = (char *)memset(progname, '\0', (i + 1));
    progname = (char *)memcpy(progname, argument, i);

    return(progname);
}

void add_heap (heap_t *heap, void *address) {
    (heap->heap_size)++;
    heap->heap_array = realloc(heap->heap_array, (heap->heap_size) * sizeof(heap_t));
    heap->heap_array[heap->heap_size - 1] = address;
}

void init_heap (heap_t **heap) {
    *heap = (heap_t *)malloc(sizeof(heap_t));
    (*heap)->heap_size = 0;
    (*heap)->heap_array = NULL;
}

void free_heap (heap_t *heap) {
    int i;

    for (i=0; i < heap->heap_size; i++) {
        free(heap->heap_array[i]);
    }
    free(heap->heap_array);
    free(heap);
}

char **get_filenames (heap_t *heap, char *progname, char *argv[]) {
    char **filenames = NULL;
    const char SOURCE_EXT[] = ".c", ERROR_EXT[] = ".err";

    if (progname == NULL) {
        return(NULL);
    }

    filenames = (char **)malloc(5 * sizeof(char **));
    add_heap(heap, filenames);
    filenames[SOURCE] = strdup(progname);
    filenames[SOURCE] = (char *)realloc(filenames[SOURCE], (strlen(filenames[SOURCE]) + sizeof(SOURCE_EXT)) * sizeof(char));
    add_heap(heap, filenames[SOURCE]);
    filenames[SOURCE] = strcat(filenames[SOURCE], SOURCE_EXT);
    
    filenames[ARGUMENTS] = argv[2];
    filenames[INPUT] = argv[3];
    filenames[OUTPUT] = argv[4];

    filenames[ERROR] = strdup(progname);
    filenames[ERROR] = (char *)realloc(filenames[ERROR], (strlen(filenames[ERROR]) + sizeof(ERROR_EXT)) * sizeof(char));
    add_heap(heap, filenames[ERROR]);
    filenames[ERROR] = strcat(filenames[ERROR], ERROR_EXT);

    return(filenames);
}

int main (int argc, char *argv[]) {
    int fd, p1, p2, p3, warnings, pipefd[2];
    char *progname = NULL, *current_dir = NULL, **filenames = NULL;
    heap_t *heap = NULL;

    if (argc != 6) {
        printf("Invalid arguments.\n Usage: ./hw4 <progname>.c <progname>.args <progname>.in <progname>.out <timeout>\n");
        return(1);
    }

    init_heap(&heap);

    progname = get_progname(argv[1]);
    add_heap(heap, progname);
    filenames = get_filenames(heap, progname, argv);

    p1 = fork();

    if (p1 == 0) {
        fd = open(filenames[ERROR], O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
        if (fd == -1) {
            printf("Could not open file.\n");
            exit(-1);
        }
        //olderr = dup(STDERR_FILENO);
        dup2(fd, STDERR_FILENO);
        execl("/usr/bin/gcc", "gcc", "-Wall", argv[1], "-o", progname, NULL);
        close(fd);
        exit(0);
        //dup2(olderr, STDERR_FILENO);
    } else if (p1 == -1) {
        printf("Error creating p1.\n");
    }

    waitpid(p1, NULL, 0);

    if (has_error(filenames[ERROR]) != 0) {
        warnings = num_warnings(filenames[ERROR]);
    } else {
        //ERROR IN *.ERR
    }

    if (pipe(pipefd) != 0) {
        printf("Error creating pipe.\n");
    }

    p2 = fork();

    if (p2 == 0) {
        fd = open(filenames[INPUT], O_RDONLY);
        dup2(fd, STDIN_FILENO);
        dup2(pipefd[1], STDOUT_FILENO);
        current_dir = (char *)calloc(strlen(progname) + 3, sizeof(char));
        current_dir = strcpy(current_dir, "./");
        current_dir = strcat(current_dir, progname);
        printf("exec %s %s %s\n", current_dir, progname, filenames[ARGUMENTS]);
        execv(current_dir, read_arguments(filenames[ARGUMENTS], progname));
        close(pipefd[1]);
        exit(0);
    } else if (p2 == -1) {
        printf("Error creating p2.\n");
    } else {
        p3 = fork();
        if (p3 == 0) {
            dup2(pipefd[0], STDIN_FILENO);
            execl("./p4diff", "p4diff", filenames[OUTPUT], NULL);
            exit(0);
        } else if (p3 == -1) {
            printf("Error creating p3.\n");
        }
    }

    waitpid(p2, NULL, 0);
    waitpid(p3, NULL, 0);

    free_heap(heap);

    return(0);
}