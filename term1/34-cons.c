#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MSGSIZE 32

int main(void) {
	key_t key = getuid();
	int flg = IPC_CREAT | 0666;
	char *a;
	int semid, shmid;
	struct sembuf consumed = {1, 1, 0};
	struct sembuf ready = {0, -1, 0};

	if ((shmid = shmget(key, MSGSIZE, flg)) == -1) {
		perror("consumer shmget");
		exit(1);
	}

	if ((semid = semget(key, 2, flg)) == -1) {
		perror("consumer semget");
		exit(1);
	}
	
	if ((a = shmat(shmid, 0, SHM_RDONLY)) == 0) {
		perror("consumer shmat");
		exit(1);
	}

	while(1) {
		int ret = semop(semid, &ready, 1);
		if (ret == -1) {
			perror("finish");
			break;
		}
		printf("%d got '%s'\n", getpid(), a);
		semop(semid, &consumed, 1);
	}

	shmdt(a);
	return 0;
}
