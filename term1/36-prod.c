#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

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
	struct sembuf sembuffer;

	if ((shmid = shmget(key, BUFSIZ, flg)) == -1) {
		perror("producer shmget");
		exit(1);
	}

	if ((semid = semget(key, 3, flg)) == -1) {
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

	for (i = 0; i < 3; i++)
		semctl(semid, i, SETVAL, 0);
	
	i = 1;
	while(1) {
		int total = 0;
		int ret = 0;
		
		fgets(a, BUFSIZ, stdin);

		if (strcmp(a, "\n") == 0)
			ret = 1;
		else
			a[strlen(a)-1] = '\0';
		
		total = semctl(semid, 2, GETVAL, 0);
		
		if (total == 0)
			break;

		/*
		sembuffer.sem_num = 0;
		sembuffer.sem_op = 0;
		sembuffer.sem_flg = 0;
		if (semop(semid, &sembuffer, 1) == -1) {
			perror("producer 0 on 0");
			unihandler(0);
		}
		*/

		sembuffer.sem_num = 0;
		sembuffer.sem_op = total;
		sembuffer.sem_flg = 0;
		if (semop(semid, &sembuffer, 1) == -1) {
			perror("producer total on 0");
			unihandler(0);
		}

		sembuffer.sem_num = 1;
		sembuffer.sem_op = -total;
		sembuffer.sem_flg = 0;
		if (semop(semid, &sembuffer, 1) == -1) {
			perror("producer -total on 1");
			unihandler(0);
		}

		if (ret == 1)
			break;
		
		i++;
	}

	shmdt(a);	
	shmctl(shmid, IPC_RMID, 0);
	semctl(semid, 0, IPC_RMID, 0);
	return 0;
}
