#include<stdio.h>
int main(int argc, char *argv[]) {
int pid;
if (!(pid=fork())) {
printf("child: execute ./add 123 456\n");
execl("./gcc", "gcc", "hw3.c", "-c", "hw3", NULL);
perror("execl");
return(1);
}
waitpid(pid, NULL, 0);
printf("parent: child terminated\n");
return(0);
}