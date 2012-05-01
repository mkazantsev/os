#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int semid;

void unihandler(int sig) {
	semctl(semid, 0, IPC_RMID, 0);
	kill(0, SIGINT);
	exit(1);
}

int main(int argc, char *argv[]) {
	key_t key = getuid();
	int nwid;
	int i;
	char *mmbs[4];
	char buf[32];
	struct sembuf sbuf[2];

	mmbs[0] = "33part";
	mmbs[1] = "33part";
	mmbs[2] = "33part";
	mmbs[3] = "33mod";

	if (argc < 2)
		nwid = 1;
	else
		nwid = atoi(argv[1]);
	
	if ((semid = semget(key, 4, IPC_CREAT | 0666)) == -1) {
		perror("line semget");
		exit(1);
	}

	signal(SIGTERM, unihandler);
	signal(SIGINT, unihandler);
	signal(SIGHUP, unihandler);

	for (i = 0; i < 4; i++)
		semctl(semid, i, SETVAL, 0);

	sbuf[0].sem_num = 2;
	sbuf[0].sem_op = -1;
	sbuf[0].sem_flg = SEM_UNDO;
	sbuf[1].sem_num = 3;
	sbuf[1].sem_op = -1;
	sbuf[1].sem_flg = SEM_UNDO;

	for (i = 0; i < 4; i++) {
		sprintf(buf, "%d", i);
		if (fork() == 0) {
			execl(mmbs[i], mmbs[i], buf, 0);
			perror("exec");
			unihandler(0);
			exit(1);
		}
	}

	for (i = 1; i <= nwid; i++) {
		if (semop(semid, sbuf, 2) == -1) {
			perror("line semop");
			unihandler(0);
			exit(1);
		}
		printf("widget #%d done\n", i);
	}
	
	unihandler(0);
}
