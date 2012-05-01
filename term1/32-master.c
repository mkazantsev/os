#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define N_MAX 128

int qid;

void unihandler(int sig) {
	msgctl(qid, IPC_RMID, 0);
	exit(1);
}

int index(pid_t *pids, pid_t pid, int n) {
	int i;
	for (i = 0; i < n; i++)
		if (pids[i] == pid)
			return i;
	return -1;
}

int isAlive(pid_t *pids, int *alive, pid_t pid, int n) {
	int i = index(pids, pid, n);
	return alive[i];
}

int totalAlive(pid_t *pids, int *alive, int n) {
	int i;
	int alv = 0;
	for (i = 0; i < n; i++)
		if (alive[i] != 0)
			alv++;
	return alv;
}

int main(void) {
	char s[BUFSIZ];
	key_t key;
	pid_t pid;
	pid_t pids[N_MAX];
	int alive[N_MAX];
	int msgflg;
	int ret;
	int i;
	int count;
	struct msgbuf *msg = (struct msgbuf *) malloc(sizeof *msg + BUFSIZ);
	
	msgflg = 0666 | IPC_CREAT;
	key = getuid();
	qid = msgget(key, msgflg);
	if (qid == -1) {
		perror("master msgget");
		unihandler(0);
	}

	signal(SIGTERM, unihandler);
	signal(SIGINT, unihandler);
	signal(SIGHUP, unihandler);
	
	for (i = 0; i < 4; i++) {
		pid = fork();
		if (pid == -1) {
			perror("fork");
			unihandler(0);
		} else if (pid == 0) {
			execlp("./32", 0);
			perror("exec");
			unihandler(0);
		}
	}

	for (i = 0; i < N_MAX; i++)
		alive[i] = 1;

	sleep(1);
	
	i = 0;
	count = 0;
	while(msgrcv(qid, msg, BUFSIZ, 0, MSG_NOERROR) != -1) {
		if (msg->mtype == 2) {
			pids[count] = atoi(msg->mtext);
			count++;
			//printf("%d in\n", msg->mtext);
		}
		
		if (msg->mtype == 1) {
			int tmp = atoi(msg->mtext);
			alive[index(pids, tmp, count)] = 0;
			printf("%d out\n", atoi(msg->mtext));
		}
		
		if (totalAlive(pids, alive, count) == 0)
			break;
				
		if (msg->mtype > 2)
			printf("%d %s\n", msg->mtype, msg->mtext);

		memset(msg, 0, BUFSIZ);
	}

	msgctl(qid, IPC_RMID, 0);
	exit(0);
}
