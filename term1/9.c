#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>

int main(void) {
	int status;
	int i;
	pid_t pid = fork();
	int *mem;
	if (pid == 0)
		execlp("cat", "cat", "9.txt", (char *) 0);
		
	printf("1 I'm parent\n");
	mem = (int *) malloc(100000000*sizeof(int));
	for (i = 0; i < 100000000; i++)
	    mem[i] = 1;
	wait(&status);
	printf("2 I'm parent\n");
	exit(0);
}
