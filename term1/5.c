#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>

int main(void) {
	int fd, symc = 0, linesc = 1, i, lines[100], lines_len[260], req;
	char *file = "5.txt";
	char buf;
	char str[260];
	
	fd = open(file, O_RDONLY);
	
	lines[1] = 0L;
	while (read(fd, &buf, 1) != 0) {
		symc++;
		if (buf == '\n') {
			lines_len[linesc] = symc - lines[linesc];
			linesc++;
			lines[linesc] = symc;
		}
	}
	linesc--;
	
	lseek(fd, 0L, SEEK_SET);
	printf("%d %d\n", symc, linesc);
	
	//printf("Line: ");
	//scanf("%d", &req);
	while (printf("Line: ") && scanf("%d", &req)) {
		if (req <= linesc && req > 0) {
			lseek(fd, lines[req], SEEK_SET);
			read(fd, &str, lines_len[req]);
			write(1, str, lines_len[req]);
		} else {
			printf("File doesn't have such line\n");
		}
	        //printf("Line: ");
	        //scanf("%d", &req);
	}
	
	close(fd);
	return 0;
}
