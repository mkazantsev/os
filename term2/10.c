#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mp = PTHREAD_MUTEX_INITIALIZER,
		mp1 = PTHREAD_MUTEX_INITIALIZER,
		mp2 = PTHREAD_MUTEX_INITIALIZER;

void *t_func(void *arg) {
	int i;
	for (i = 0; i < 10; ) {
		if (pthread_mutex_unlock(&mp))
			continue;
		pthread_mutex_lock(&mp2);
		printf("child thread: hello\n");
		pthread_mutex_unlock(&mp2);
		i++;
	}
	return NULL;
}

int main() {
	pthread_t tid;
	int i;
	pthread_mutexattr_t mattr;
	if (pthread_mutexattr_init(&mattr) != 0) {
		perror("mutexattr_init");
		exit(1);
	}
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_ERRORCHECK);
	if (pthread_mutex_init(&mp, &mattr) != 0) {
		perror("mutex_init");
		exit(1);
	}
	if (pthread_mutex_init(&mp1, &mattr) != 0) {
		perror("mutex_init 1");
		exit(1);
	}
	if (pthread_mutex_init(&mp2, &mattr) != 0) {
		perror("mutex_init 2");
		exit(1);
	}
	
	pthread_mutex_lock(&mp);
	
	if (pthread_create(&tid, NULL, t_func, NULL) != 0) {
		perror("create");
		exit(1);
	}
	
	for (i = 0; i < 10;) {
		if (pthread_mutex_lock(&mp))
			continue;
		pthread_mutex_lock(&mp1);
		printf("parent thread: hello\n");
		pthread_mutex_unlock(&mp1);
		i++;
	}

	pthread_mutexattr_destroy(&mattr);
	pthread_mutex_destroy(&mp);
	pthread_mutex_destroy(&mp1);
	pthread_mutex_destroy(&mp2);
	pthread_exit(NULL);
	return 0;
}
