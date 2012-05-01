#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int semid;
int shmid;

void on(void) {
}

void cons_off(int sig) {
	struct sembuf sem;
	sem.sem_num = 2;
	sem.sem_op = -1;
	sem.sem_flg = 0;
	if (semop(semid, &sem, 1) == -1) {
		perror("consumer -1 on 2");
	}
	exit(0);
}

void cons_on(void) {
	struct sembuf sem;
	sem.sem_num = 2;
	sem.sem_op = 1;
	sem.sem_flg = 0;
	if (semop(semid, &sem, 1) == -1) {
		perror("consumer 1 on 2");
		exit(1);
	}
}

int main(void) {
	key_t key = getuid();
	int flg = 0;
	char *a;
	struct sembuf sembuffer;

	if ((shmid = shmget(key, BUFSIZ, flg)) == -1) {
		perror("consumer shmget");
		exit(1);
	}

	if ((semid = semget(key, 3, flg)) == -1) {
		perror("consumer semget");
		exit(1);
	}
	
	if ((a = shmat(shmid, 0, SHM_RDONLY)) == 0) {
		perror("consumer shmat");
		exit(1);
	}

	signal(SIGTERM, cons_off);
	signal(SIGHUP, cons_off);
	signal(SIGINT, cons_off);
	
	cons_on();
	
	while (1) {
		int ret = 0;

		sembuffer.sem_num = 0;
		sembuffer.sem_op = -1;
		sembuffer.sem_flg = 0;		
		if (semop(semid, &sembuffer, 1) == -1) {
			perror("consumer -1 on 0");
			exit(1);
		}

		if (strcmp(a, "\n") == 0) {
			ret = 1;
			printf("%d got EOF\n", getpid());
		} else {
			printf("%d got '%s'\n", getpid(), a);
		}
		
		sembuffer.sem_op = 0;
		if (semop(semid, &sembuffer, 1) == -1) {
			perror("consumer 0 on 0");
			exit(1);
		}

		sembuffer.sem_num = 1;
		sembuffer.sem_op = 1;
		if (semop(semid, &sembuffer, 1) == -1) {
			perror("consumer 1 on 1");
			exit(1);
		}

		if (ret)
			break;
	}

	shmdt(a);
	cons_off(0);
}
