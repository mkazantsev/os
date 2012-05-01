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

int main(void) {
	char s[BUFSIZ];
	key_t key;
	pid_t pid;
	pid_t pids[N_MAX];
	pid_t alive[N_MAX];
	int count = 0;
	int msgflg;
	int ret;
	int i;
	struct msgbuf *msg = (struct msgbuf *) malloc(sizeof *msg + BUFSIZ);
	
	msgflg = 0666 | IPC_CREAT;
	key = getuid();
	qid = msgget(key, msgflg);
	if (qid == -1) {
		perror("msgget");
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
			execlp("./31rec", 0);
			perror("exec");
			unihandler(0);
		} /*else
			pids[i] = pid;*/
	}

	sleep(1);

	for (i = 0; i < N_MAX; i++)
		alive[i] = 1;
	
	while (1) {
		int alv = 0;
		
		fgets(s, BUFSIZ, stdin);
		
		while (msgrcv(qid, msg, BUFSIZ, 1, MSG_NOERROR | IPC_NOWAIT) != -1) {
			int tmp = atoi(msg->mtext);
			int i;
			for (i = 0; i < count; i++)
				if (pids[i] == tmp)
					alive[i] = 0;
		}
		
		while (msgrcv(qid, msg, BUFSIZ, 2, MSG_NOERROR | IPC_NOWAIT) != -1) {
			pids[count] = atoi(msg->mtext);
			count++;
		}

		for (i = 0; i < count; i++) {
			if (alive[i] == 1)
				alv++;
		}

		if (alv == 0)
			break;
		
		ret = 0;
		if (strcmp(s, "\n") == 0)
			ret = 1;
		
		strcpy(msg->mtext, s);
		msg->mtext[strlen(msg->mtext)-1] = '\0';
		if (ret == 1)
			strcpy(msg->mtext, ".");
		
		for (i = 0; i < count; i++) {
			if (alive[i]) {
				msg->mtype = pids[i];
				if (msgsnd(qid, msg, strlen(msg->mtext), 0) == -1) {
					perror("msgsnd");
					unihandler(0);
				}
			}
		}

		if (ret == 1)
			break;
	}

	sleep(1);
	
	for (i = 0; i < count; i++) {
		if (alive[i]) {
			if (msgrcv(qid, msg, BUFSIZ, 1, MSG_NOERROR) < 0) {
				perror("msgrcv");
				unihandler(0);
			}
		}
	}

	msgctl(qid, IPC_RMID, 0);
	exit(0);
}
