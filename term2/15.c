#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	pid_t pid;
	if ((pid = fork()) == -1) {
		perror("fork");
		exit(1);
	}
	if (pid == 0)
		if (execvp("./15-1", argv) == -1) {
			perror("execvp 1");
			exit(1);
		}
	else
		if (execvp("./15-2", argv) == -1) {
			perror("execvp 2");
			exit(1);
		}
	return 0;
}
