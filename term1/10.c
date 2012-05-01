#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

int main(int argc, char *argv[]) {
	pid_t pid = fork();
	int i;
	int exitstatus;
	if (pid == 0) {
		printf("New [%d]\n", getpid());
		execvp(argv[1], &argv[1]);
		exit(111);
	}
	while(1);
	wait(&exitstatus);
	printf("[%d] status is: %d\n", getpid(), WEXITSTATUS(exitstatus));
	exit(0);
}
