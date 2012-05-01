#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(void) {
	FILE *p;
	int status;
	pid_t child;
	char *s = "hElLo wOrLd!";

	p = popen("tr [:lower:] [:upper:]", "w");

	fwrite(s, sizeof(char), strlen(s), p);

	pclose(p);
	write(1, "\n", 1);
	return 0;
}
