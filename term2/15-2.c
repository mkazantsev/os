#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

sem_t *sems[2];

int main(void) {
	pthread_t tid;
	int i;
	int flg = O_CREAT;
	char *names[] = {"/outsem1", "/outsem2"};
	for (i = 0; i < 2; i++) {
		if ((sems[i] = sem_open(names[i], flg, 0777, 1-i)) == SEM_FAILED) {
			perror("sem_open");
			exit(1);
		}
	}

	for (i = 0; i < 10; i++) {
		if (sem_wait(sems[1])) {
			perror("sem_wait");
			exit(1);
		}
		printf("second process hello\n");
		if (sem_post(sems[0])) {
			perror("sem_post");
			exit(1);
		}
	}

	for (i = 0; i < 2; i++) {
		sem_close(sems[i]);
	}
	pthread_exit(NULL);
	return 0;
}
