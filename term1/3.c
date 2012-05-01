#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
	FILE *f;
	printf("uid: %d\neuid: %d\n", getuid(), geteuid());
	if (f = fopen("3.txt", "r")) {
		fclose(f);
	} else {
		perror("File wasn't open: ");
	}
	
	seteuid(getuid());
	printf("uid: %d\neuid: %d\n", getuid(), geteuid());
	if (f = fopen("3.txt", "r")) {
		fclose(f);
	} else {
		perror("File wasn't open: ");
	}
	return 0;
}
