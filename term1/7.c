#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>


int main(void) {
	int fd, linesc = 1, i, lines[100], lines_len[260];
	int req;
	int fsize;
	char *pa;
	char *file = "5.txt";
	char buf;
	char str[32];
	struct pollfd fds[1];
	
	fd = open(file, O_RDONLY
	fsize = lseek(fd, 0L, SEEK_END);
	pa = mmap(0, fsize, PROT_READ, MAP_PRIVATE, fd, 0);
	
	lines[1] = 0L;
	for (i = 0; i < fsize; i++) {
		if (pa[i] == '\n') {
			lines_len[linesc] = i - lines[linesc];
			linesc++;
			lines[linesc] = i;
		}
	}
	linesc--;
	
	printf("%d %d\n", fsize, linesc);

	fds[0].fd = 0;
	fds[0].events = POLLIN;
	fds[0].revents = 0;
	
	while (printf("Line: (5 secs to enter)\n")) {
		if (!poll(fds, 1, 5000)) {
			write(1, pa, fsize);
			break;
		} else {
			i = read(0, str, 32);
			str[i] = '\0';
			req = atoi(str);
			
			if (req <= linesc && req > 0) {
				write(1, pa + lines[req], lines_len[req]);
			} else {
				printf("Not a line\n");
			}
		}
	}
	munmap(pa, fsize);
	close(fd);
	return 0;
}
