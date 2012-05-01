#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *t_func(void *arg) {
	int i;
	for (i = 0; i < 10; i++)
		printf("child thread: hello\n");
}

int main() {
	pthread_t tid;
	int i;
	if (pthread_create(&tid, NULL, t_func, NULL) != 0) {
		perror("create");
		exit(1);
	}
	sleep(1);
	if (pthread_join(tid, NULL) != 0) {
		perror("join");
		exit(1);
	}
	if (pthread_join(tid, NULL) != 0) {
	       perror("join 2");
		exit(1);
	}
	for (i = 0; i < 10; i++)
		printf("parent thread: hello\n");
	pthread_exit(NULL);
	return 0;
}
