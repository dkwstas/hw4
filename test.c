#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

static void timer_handler (int signal) {
    write(STDOUT_FILENO, "Timer went off.\n", 17);
}

int main (void) {
    int p1;
    //struct itimerval timer;
    struct sigaction action;

    printf("%d\n", getpid());

    p1 = fork();
    if (p1 == 0) {
        printf("Sleeping...\n");
        sleep(5);
        printf("BYE\n");
        return(0);
    } else {
        action.sa_handler = timer_handler;
        sigaction(SIGALRM, &action, NULL);
        printf("WP:%d\n", waitpid(p1, NULL, 0));
        printf("%s\n", strerror(errno));
        kill(p1, SIGKILL);
    }

    

    //timer.it_value.tv_sec = 20;
    //timer.it_value.tv_usec = 0;
    //timer.it_interval.tv_sec = 0;
    //timer.it_interval.tv_usec = 0;

    //printf("%d\n", setitimer(ITIMER_REAL, &timer, NULL));
    //printf("%s\n", strerror(errno));

    //while (1) {
    //    printf("Sleeping... (%d)\n", i);
    //    sleep(1);
    //    i++;
    //}

    return(0);
}