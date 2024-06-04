#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
int fd[2]; char c; char str[64];
pipe(fd);
if (!fork()) {
dup2(fd[1], STDOUT_FILENO); /* redirect stdout */
close(fd[0]); close(fd[1]);
execl("./add", "add", "123", "456", NULL);
return(1);
}
dup2(fd[0], STDIN_FILENO); /* redirect stdin */
close(fd[0]); close(fd[1]);
while (scanf("%63s", str) != EOF) {
printf("%s\n", str);
}
return(0);
}