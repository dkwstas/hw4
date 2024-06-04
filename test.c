#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>

static void timer_handler (int signal) {
    printf("Timer went off.\n");
}

int main (void) {
    int i = 0;
    struct itimerval timer;
    struct sigaction action;

    action.sa_handler = timer_handler;
    sigaction(SIGALRM, &action, NULL);

    timer.it_value.tv_sec = 20;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;

    printf("%d\n", setitimer(ITIMER_REAL, &timer, NULL));
    printf("%s\n", strerror(errno));

    while (1) {
        printf("Sleeping... (%d)\n", i);
        sleep(1);
        i++;
    }

    return(0);
}