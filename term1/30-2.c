#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* strlen */

int main(void) {
	int ret;
	key_t key;
	int msgflg = 0;
	int qid;
	int mid = 0;
	struct msgbuf *rcvbuf = (struct msgbuf *) malloc(sizeof *rcvbuf + BUFSIZ);

	key = getuid();
	qid = msgget(key, msgflg);
	
	while ((ret = msgrcv(qid, rcvbuf, BUFSIZ, mid, MSG_NOERROR | IPC_NOWAIT)) > 0) {
		printf("%s %d %d\n", rcvbuf->mtext, rcvbuf->mtype, ret);
	}

	msgctl(qid, IPC_RMID, 0);
	return 0;
}
