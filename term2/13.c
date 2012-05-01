#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

sem_t sems[2];

void *t_func(void *arg) {
	int i;
	for (i = 0; i < 10; i++) {
		if (sem_wait(sems+1)) {
			perror("sem_wait");
			exit(1);
		}
		printf("child thread: hello\n");
		if (sem_post(sems)) {
			perror("sem_post");
			exit(1);
		}
	}
	return NULL;
}

int main(void) {
	pthread_t tid;
	int i;
	for (i = 0; i < 2; i++) {
		if (sem_init(sems+i, 0, i) != 0) {
			perror("sem_init");
			exit(1);
		}
	}

	if (pthread_create(&tid, NULL, t_func, NULL) != 0) {
		perror("create");
		exit(1);
	}
	
	for (i = 0; i < 10; i++) {
		if (sem_wait(sems)) {
			perror("sem_wait");
			exit(1);
		}
		printf("parent thread: hello\n");
		if (sem_post(sems+1)) {
			perror("sem_post");
			exit(1);
		}
	}

	for (i = 0; i < 2; i++)
		sem_destroy(sems+i);
	pthread_exit(NULL);
	return 0;
}
