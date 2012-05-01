#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *t_func(void *arg) {
	int i;
	for (i = 0; i < 10; i++)
		printf("child thread: hello\n");
	return NULL;
}

int main() {
	pthread_t tid;
	int i;
	if (pthread_create(&tid, NULL, t_func, NULL) != 0) {
		perror("creation");
		exit(1);
	}
	for (i = 0; i < 10; i++)
		printf("parent thread: hello\n");
	pthread_exit(NULL);
	return 0;
}
