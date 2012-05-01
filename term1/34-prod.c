#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define MSGSIZE 32
#define MSGNUM 10

int shmid;
int semid;

void unihandler(int sig) {
	shmctl(shmid, IPC_RMID, 0);
	semctl(semid, 0, IPC_RMID, 0);
	exit(1);
}

int main(void) {
	key_t key = getuid();
	int flg = IPC_CREAT | 0666;
	char *a;
	int i;
	struct sembuf produced = {0, 1, 0};
	struct sembuf ready = {1, -1, 0};

	if ((shmid = shmget(key, MSGSIZE, flg)) == -1) {
		perror("producer shmget");
		exit(1);
	}

	if ((semid = semget(key, 2, flg)) == -1) {
		perror("producer semget");
		shmctl(shmid, IPC_RMID, 0);
		exit(1);
	}
	
	if ((a = shmat(shmid, 0, 0)) == (char *) -1) {
		perror("producer shmat");
		unihandler(0);
	}

	signal(SIGTERM, unihandler);
	signal(SIGHUP, unihandler);
	signal(SIGINT, unihandler);

	for (i = 0; i < 2; i++)
		semctl(semid, i, SETVAL, 0);

	for (i = 0; i < MSGNUM; i++) {
		if (i > 0)
			semop(semid, &ready, 1);
		sprintf(a, "message #%d", i);
		semop(semid, &produced, 1);
	}

	shmdt(a);
	
	shmctl(shmid, IPC_RMID, 0);
	semctl(semid, 0, IPC_RMID, 0);
	return 0;
}
