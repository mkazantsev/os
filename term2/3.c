#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define TNUM 4

void *t_func(void *arg) {
	int i;
	char **s;
	char **carg = (char **) arg;
	for (s = carg; *s != NULL; s++)
		printf("%s ", *s);
	printf("\n");
}

int main(void) {
	pthread_t tid[TNUM];
	char *args[][20] = {
		{"thread1 message1", "thread1 message2", NULL},
		{"thread2 message1", NULL},
		{"thread3 message1", "thread3 message2", "thread3 message3", NULL},
		{"thread4 message1", "thread4 message2", NULL}
	};	
	int i;

	for (i = 0; i < TNUM; i++)
		if (pthread_create(tid+i, NULL, t_func, args[i]) != 0) {
			perror("create");
			exit(1);
		}
	for (i = 0; i < TNUM; i++)
		if (pthread_join(tid[i], NULL) != 0) {
			perror("join");
			exit(1);
		}
	pthread_exit(NULL);
	return 0;
}
