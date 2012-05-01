#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void *reader(void *arg) {
	pthread_rwlock_rdlock(&rwlock);
	printf("reader 2\n");
	pthread_rwlock_unlock(&rwlock);
}

void *writer(void *arg) {
	pthread_rwlock_wrlock(&rwlock);
	printf("writer\n");
	pthread_rwlock_unlock(&rwlock);
}

int main(void) {
	pthread_t tid[2];
	
	pthread_rwlock_rdlock(&rwlock);
	if (pthread_create(tid, NULL, writer, NULL)) {
		perror("create");
		exit(1);
	}
	pthread_detach(tid[0]);
	
	sleep(1);
	
	if (pthread_create(tid+1, NULL, reader, NULL)) {
		perror("create");
		exit(1);
	}
	pthread_detach(tid[1]);

	sleep(3);
	printf("reader 1\n");

	pthread_rwlock_unlock(&rwlock);
	pthread_rwlock_destroy(&rwlock);
	pthread_exit(NULL);
	return 0;
}
