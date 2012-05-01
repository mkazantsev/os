#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(void) {
	char *fname = "5.txt";
	int fd;
	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	
	if ((fd = open(fname, O_RDWR)) == -1) {
	    perror("open");
	    return 1;
	}
	if (fcntl(fd, F_SETLK, &lock) != -1)
		system("vi 5.txt");
	else 
		perror("fcntl");

	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);
	close(fd);
	return 0;
}
