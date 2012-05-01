#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* strlen */

int main(void) {
	key_t key;
	int qid;
	int msgflg;
	char *s = "Test";
	int ret;
	int i;
	struct msgbuf *sndbuf = (struct msgbuf *) malloc(sizeof *sndbuf + BUFSIZ);
	//struct msgbuf *rcvbuf = (struct msgbuf *) malloc(sizeof *rcvbuf + BUFSIZ);

	msgflg = 0666 | IPC_CREAT;
	
	//key = ftok("/etc/hosts", 1);
	key = getuid();
	qid = msgget(key, msgflg);
	sndbuf->mtype = 1;
	sprintf(sndbuf->mtext, "%s", s);

	for (i = 0; i < 10; i++) {
		msgsnd(qid, sndbuf, strlen(s) + 1, IPC_NOWAIT);
	}
	
	//msgctl(qid, IPC_RMID, 0);
	return 0;
}
