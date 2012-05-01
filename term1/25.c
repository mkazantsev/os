#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(void) {
	int fd[2];
	int status;
	pid_t child;
	char buf;
	char buf2[10];

	pipe(fd);

	child = fork();
	if (child == 0) {
		char *s = "hElLo wOrLd!";
		//close(fd[0]);
		write(fd[0], "hello", 5);
		write(fd[1], s, strlen(s));
		return 0;
	}
	
	//close(fd[1]);
	read(fd[1], buf2, 10);
	
	write(1, buf2, 10);
	
	close(fd[1]);
	
	while ((read(fd[0], &buf, 1)) != 0) {
		buf = toupper(buf);
		write(1, &buf, 1);
	}
	write(1, "\n", 1);
	wait(&status);
	return 0;
}
