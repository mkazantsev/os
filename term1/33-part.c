#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	key_t key = getuid();
	int semid;
	int partno;
	struct sembuf sbuf;
	
	partno = atoi(argv[1]);
	
	if ((semid = semget(key, 4, 0)) == -1) {
		perror("part semget");
		exit(1);
	}

	sbuf.sem_num = partno;
	sbuf.sem_op = 1;
	sbuf.sem_flg = SEM_UNDO;
	
	while(1) {
		sleep(partno + 2);
		printf("part '%c' done\n", partno + 'a');
		if (semop(semid, &sbuf, 1) == -1) {
			perror("semop");
			exit(1);
		}
	}
}
