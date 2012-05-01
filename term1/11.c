#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int execvpe(char *path, char **argv, char **envp) {
    extern char **environ;
    environ = envp;
    execvp(path, argv);
    return 1;
}

int main(void) {
	char *argv[] = {"env", (char *) 0};
	char *env[] = {"A=abc", "B=bcd", "C=cde", (char *) 0};
	execvpe(argv[0], argv, env);
	exit(1);
}
