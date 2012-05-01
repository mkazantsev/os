#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *c_func(void *arg) {
	int **iarg = (int **) arg;
	printf("handler pointers: %d, %d\n", iarg[0], iarg[1]);
	free(iarg[0]);
	free(iarg[1]);
	printf("Cancellation handler\n");
}

void *t_func(void *arg) {
	int *a = malloc(10 * sizeof(int));
	int *b = malloc(20 * sizeof(int));
	//int **c_arg = malloc(2 * sizeof(int *));
	int *c_arg[] = {a, b};
	printf("thread pointers: %d, %d\n", c_arg[0], c_arg[1]);
	pthread_cleanup_push(c_func, c_arg);
	while (1) {
		sleep(1);
		//write(0, "thread text\n", 12);
	}
	pthread_cleanup_pop(0);
	//free(c_arg);
	return NULL;
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
