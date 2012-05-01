#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h> /* strlen */

void unihandler(int sig) {
	int qid = msgget(getuid(), 0);
	struct msgbuf *msg = (struct msgbuf *) malloc(sizeof *msg + BUFSIZ);
	msg->mtype = 1;
	sprintf(msg->mtext, "%d", getpid());
	if (msgsnd(qid, msg, strlen(msg->mtext), 0) == -1) {
		perror("sender msgsnd");
	}
	exit(1);
}

int main(void) {
	int ret;
	key_t key;
	pid_t pid;
	int msgflg = 0;
	int qid;
	char buf[BUFSIZ];
	int i;
	struct msgbuf *msg = (struct msgbuf *) malloc(sizeof *msg + BUFSIZ);

	key = getuid();
	qid = msgget(key, msgflg);
	if (qid == -1) {
		perror("sender msgget");
		exit(1);
	}

	signal(SIGTERM, unihandler);
	signal(SIGHUP, unihandler);
	signal(SIGINT, unihandler);
	
	pid = getpid();

	msg->mtype = 2;
	sprintf(msg->mtext, "%d", pid);
	if (msgsnd(qid, msg, strlen(msg->mtext), 0) == -1) {
		perror("sendet msgsend");
		exit(1);
	}
	
	for(i = 0; i < 5; i++) {
		msg->mtype = pid;
		sprintf(msg->mtext, "%s %d", "OK", i);
		//printf("'%s'\n", msg->mtext);
		if (msgsnd(qid, msg, strlen(msg->mtext), 0) == -1) {
			perror("sender msgsnd");
			exit(1);
		}
		sleep(1);
	}

	msg->mtype = 1;
	sprintf(msg->mtext, "%d", pid);
	if (msgsnd(qid, msg, strlen(msg->mtext), 0) == -1) {
		perror("sender msgsend");
		exit(1);
	}
	return 0;
}
