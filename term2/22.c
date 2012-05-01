#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h> /* exit */
#include <stdio.h> /* printf */
#include <unistd.h> /* sleep */
#include <signal.h> /* sigset */

sem_t sems[4];
char *names[] = {"mod", "a", "b", "c"};

void sem_handler(int sig) {
	int i;
	for (i = 0; i < 4; i++)
		sem_destroy(sems+i);
}

void *t_detail(void *arg) {
	int i = (int) arg;
	while (1) {
		if (i == 0) {
			if (sem_wait(sems+1) != 0) {
				perror("sem_wait");
				exit(1);
			}
			if (sem_wait(sems+2) != 0) {
				perror("sem_wait");
				exit(1);
			}			
			if (sem_post(sems) != 0) {
				perror("sem_post");
				exit(1);
			}
		} else {
			sleep(i);
			if (sem_post(sems+i) != 0) {
				perror("sem_post");
				exit(1);
			}
		}
		printf("%s done\n", names[i]);
	}
	return NULL;
}

int main(void) {
	pthread_t tids[4];
	int i;
	for (i = 0; i < 4; i++) {
		if (pthread_create(tids+i, NULL, t_detail, (void *)i) != 0) {
			perror("create");
			exit(1);
		}
		pthread_detach(tids[i]);
	}
	
	for (i = 0; i < 4; i++) {
		if (sem_init(sems+i, 0, 0) != 0) {
			perror("sem_init");
			exit(1);
		}
	}

	sigset(SIGINT, sem_handler);
	
	while (1) {
		if (sem_wait(sems+3) != 0) {
			perror("sem_wait");
			exit(1);
		}
		if (sem_wait(sems) != 0) {
			perror("sem_wait");
			exit(1);
		}
		printf("widget done\n");
	}
	sem_handler(0);
	pthread_exit(NULL);
	return 0;
}
