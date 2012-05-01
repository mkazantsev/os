#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* strlen */
#include <signal.h>

void unihandler(int sig) {
	struct msgbuf *msg = (struct msgbuf *) malloc(sizeof *msg + BUFSIZ);
	key_t key = getuid();
	int qid = msgget(key, 0);

	if (qid == -1)
		exit(1);
	
	msg->mtype = 1;
	sprintf(msg->mtext, "%d", getpid());
	if (msgsnd(qid, msg, strlen(msg->mtext), 0) == -1) {
		perror("msgsnd");
	}
	exit(1);
}

int main(int argc, char **argv) {
	int ret;
	key_t key;
	pid_t pid;
	int msgflg = 0;
	int qid;
	char buf[BUFSIZ];
	struct msgbuf *rcvbuf = (struct msgbuf *) malloc(sizeof *rcvbuf + BUFSIZ);
	struct msgbuf *sndbuf = (struct msgbuf *) malloc(sizeof *sndbuf + BUFSIZ);

	key = getuid();
	qid = msgget(key, msgflg);

	if (qid == -1) {
		perror("msgget");
		exit(1);
	}

	pid = getpid();

	signal(SIGTERM, unihandler);
	signal(SIGINT, unihandler);
	signal(SIGHUP, unihandler);
	
	sndbuf->mtype = 2;
	sprintf(sndbuf->mtext, "%d", pid);
	if (msgsnd(qid, sndbuf, strlen(sndbuf->mtext), 0) == -1) {
		perror("msgsnd");
		exit(1);
	}
	
	while (1) {
		memset(rcvbuf, 0, BUFSIZ);
		if ((ret = msgrcv(qid, rcvbuf, BUFSIZ, pid, MSG_NOERROR)) == -1) {
			perror("msgrcv");
			exit(1);
		}
		if (strcmp(rcvbuf->mtext, ".") == 0) {
			sndbuf->mtype = 1;
			strcpy(sndbuf->mtext, "OK");
			if (msgsnd(qid, sndbuf, strlen(sndbuf->mtext), 0) == -1) {
				perror("msgsnd");
				exit(1);
			}
			break;
		} else {
			printf("%d %s\n", pid, rcvbuf->mtext);
		}
	}
	return 0;
}
