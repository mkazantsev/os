#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
	FILE *p;
	int fd;
	char *fn = "27.c";
	char c1, c2 = '\n';
	
	p = popen("wc -l", "w");
	fd = open(fn, O_RDONLY);
	
	while (read(fd, &c1, 1) == 1) {
		if (c1 == '\n' && c2 == '\n')
			fputc('\n', p);


		c2 = c1;
	}
	if (c1 == '\n')
		fputc('\n', p);
	
	close(fd);
	pclose(p);
	return 0;
}
