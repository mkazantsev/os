#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char *argv[]) {
	key_t key = getuid();
	int semid;
	int i;
	struct sembuf sbuf[2];
	struct sembuf sbuf2;
	
	if ((semid = semget(key, 4, 0)) == -1) {
		perror("mod semget");
		exit(1);
	}

	sbuf[0].sem_num = 0;
	sbuf[0].sem_op = -1;
	sbuf[0].sem_flg = SEM_UNDO;
	sbuf[1].sem_num = 1;
	sbuf[1].sem_op = -1;
	sbuf[1].sem_flg = SEM_UNDO;

	sbuf2.sem_num = 3;
	sbuf2.sem_op = 1;
	sbuf2.sem_flg = SEM_UNDO;
	
	while(1) {
		if (semop(semid, sbuf, 2) == -1) {
			perror("mod semop");
			exit(1);
		}
		printf("module done\n");
		if (semop(semid, &sbuf2, 1) == -1) {
			perror("mod semop");
			exit(1);
		}
	}
}
