#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>

int main(void) {
	int fd, symc = 0, linesc = 1, i, lines[100], lines_len[260], req;
	char *file = "5.txt";
	char buf;
	char str[32];
	char input[32];

	struct pollfd fds[1];
	
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

	fds[0].fd = 0;
	fds[0].events = POLLIN;
	fds[0].revents = 0;
	
	while (printf("Line: (5 secs to enter)\n")) {
		if (!poll(fds, 1, 5000)) {
			lseek(fd, 0L, SEEK_SET);
			while (read(fd, str, 32)) {
				write(1, str, 32);
				memset(str, 0, 32);
			}
			break;
		} else {
			i = read(0, str, 32);
			str[i] = '\0';
			req = atoi(str);
			
			memset(str, 0, 32);			
			if (req <= linesc && req > 0) {
			    lseek(fd, lines[req], SEEK_SET);
			    read(fd, &str, lines_len[req]);
			    write(1, str, lines_len[req]);
			} else {
			    printf("Not a line\n");
			}
		}
	}
	close(fd);
	return 0;
}
