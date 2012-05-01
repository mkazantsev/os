#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mp = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
int predicate = 0;

void *t_func(void *arg) {
	int i;
	pthread_mutex_lock(&mp);
	for (i = 0; i < 10; i++) {
		while (!predicate) {
			if (pthread_cond_wait(&cv, &mp)) {
				perror("wait");
				exit(1);
			}
		}
		printf("child thread: hello\n");
		predicate = 0;
		if (pthread_cond_signal(&cv)) {
			perror("signal");
			exit(1);
		}
	}
	pthread_mutex_unlock(&mp);
	pthread_cond_signal(&cv);
	return NULL;
}

int main(void) {
	pthread_t tid;
	int i;

	pthread_mutex_lock(&mp);
	
	if (pthread_create(&tid, NULL, t_func, NULL) != 0) {
		perror("create");
		exit(1);
	}

	for (i = 0; i < 10; i++) {
		//pthread_mutex_lock(&mp);
		/*if (pthread_cond_signal(&cv)) {
			perror("signal");
			exit(1);
		}*/
		while (predicate) {
			if (pthread_cond_wait(&cv, &mp)) {
				perror("wait");
				exit(1);
			}
		}
		printf("parent thread: hello\n");
		predicate = 1;
		if (pthread_cond_signal(&cv)) {
			perror("signal");
			exit(1);
		}
	}
	pthread_mutex_unlock(&mp);
	pthread_cond_signal(&cv);

	pthread_join(tid, NULL);
	pthread_mutex_destroy(&mp);
	pthread_cond_destroy(&cv);
	pthread_exit(NULL);
	return 0;
}
