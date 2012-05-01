#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main(void) {
	int flg = fcntl(0, F_GETFL);
	printf("%x\n", flg);
	flg = flg | MSG_OOB;
	printf("%x\n", flg);
	return 0;
}
