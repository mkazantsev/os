#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *t_func(void *arg) {
	while (1) {
		pthread_testcancel();
		//write(0, "thread text\n", 12);
		printf("thread text\n");
	}
}

int main() {
	pthread_t tid;
	int i;
	if (pthread_create(&tid, NULL, t_func, NULL) != 0) {
		perror("create");
		exit(1);
	}
	pthread_detach(tid);
	sleep(2);
	if (pthread_cancel(tid) != 0) {
		perror("cancel");
		exit(1);
	}
	pthread_exit(NULL);
	return 0;
}
