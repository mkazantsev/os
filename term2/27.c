#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <aio.h>

int delimpos(char *s, char delim) {
	int i;
	int done = 0;
	for (i = 0; i < strlen(s); i++)
		if (s[i] == delim) {
			done = 1;
			break;
		}
	return done ? i : -1;
}

int makerequest(char *s, char *dir) {
	strcpy(s, "GET ");
	strcpy(s + strlen(s), dir);
	strcpy(s + strlen(s), " HTTP/1.0\r\n\r\n");
}

int main(int argc, char **argv) {
	struct aiocb readsock, readterm, writeterm;
	int pos, rh_size, d_size;
	int lines = 0, maxlines = 25;
	int nwrote = 0, nread = 0;
	int empty = 1, full = 0, eof = 0;
	int status, sock, nbytes_sent, nbytes_recv, backlog;
	char buf[512], *remote_host, *dir, *port = "http";
	char request[255];
	char tb[10];
	struct addrinfo hints, *result, *rp;
	
	if (argc < 2) {
		printf("Usage: %s URL\n", argv[0]);
		exit(1);
	}

	pos = delimpos(argv[1], '/');
	if (pos != -1) {
		rh_size = pos + 2;
		d_size = strlen(argv[1]) - pos + 2;
	} else {
		rh_size = strlen(argv[1]) + 1;
		d_size = 2;
	}

	remote_host = (char *) malloc(rh_size * sizeof(char));
	dir = (char *) malloc(d_size * sizeof(char));
	
	if (pos != -1) {
		remote_host = strncpy(remote_host, argv[1], pos);
		remote_host[strlen(remote_host)] = '\0';
		dir = strcpy(dir, argv[1] + pos);
	} else {
		remote_host = strcpy(remote_host, argv[1]);
		dir = strcpy(dir, "/");
	}

	makerequest(request, dir);

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = IPPROTO_TCP;
	
	if ((status = getaddrinfo(remote_host, port, &hints, &result)) != 0) {
		printf("getaddrinfo: %s\n", gai_strerror(status));
		exit(1);
	}
	
	if ((sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol)) == -1) {
		perror("socket");
		exit(1);
	}

	if (connect(sock, result->ai_addr, result->ai_addrlen) == -1) {
		perror("connect");
		exit(1);
	}
	if ((nbytes_sent = write(sock, request, strlen(request))) == -1) {
		perror("send");
		exit(1);
	}

	memset(&readsock, 0, sizeof(readsock));
	readsock.aio_fildes = sock;
	memset(&writeterm, 0, sizeof(writeterm));
	writeterm.aio_fildes = 1;
	memset(&readterm, 0, sizeof(readterm));
	readterm.aio_fildes = 0;
	readterm.aio_buf = tb;

	while (1) {
		struct aiocb *reqlist[3] = {NULL, NULL, NULL};
		int nreq = 0;
		int ret;

		memset(reqlist, 0, sizeof(reqlist));

		if (!full && !eof) {
			readsock.aio_buf = buf + nread;
			if (nread >= nwrote)
				readsock.aio_nbytes = sizeof(buf) - nread;
			else
				readsock.aio_nbytes = nwrote - nread;		
			aio_read(&readsock);
			reqlist[nreq++] = &readsock;
		} else {
			readsock.aio_nbytes = 0;
		}

		if (!empty && lines < maxlines) {
			char *t = buf + nwrote;
			char *l;
			if (nwrote < nread) {
				l = buf + nread;
			} else {
				l = buf + sizeof(buf);
			}
			for(; t < l && *t != '\n'; t++);
			if (t < l) {
	               		t++;
	            		lines++;
	        	}

			writeterm.aio_buf = buf + nwrote;
			writeterm.aio_nbytes = t - (buf + nwrote);
			aio_write(&writeterm);
			reqlist[nreq++] = &writeterm;
		} else {
			writeterm.aio_nbytes = 0;
		}

		if (lines >= maxlines) {
			readterm.aio_nbytes = 1;
			aio_read(&readterm);
			reqlist[nreq++] = &readterm;
		}

		aio_suspend((const struct aiocb const * const *)reqlist, nreq, NULL);

		if (readsock.aio_nbytes != 0 && (ret = aio_error(&readsock)) != EINPROGRESS) {
			if (ret != 0) {
				printf("reading error: %s\n", strerror(ret));
				perror("read socket");
				exit(1);
			}
			ret = aio_return(&readsock);
			if (ret == 0) {
				eof = 1;
			} else {
				nread += ret;
				if (nread == sizeof(buf)) nread = 0;
				if (nread == nwrote) full = 1;
				empty = 0;
			}
		}

		if (writeterm.aio_nbytes != 0 && (ret = aio_error(&writeterm)) != EINPROGRESS) {
			if (ret != 0) {
				printf("writing to terminal error\n");
				exit(1);
			}
			ret = aio_return(&writeterm);
			if (ret == 0) {
				printf("writing to terminal error\n");
				exit(1);
			}
			nwrote += ret;
			if (nwrote == sizeof(buf)) nwrote = 0;
			if (nwrote == nread) empty = 1;
			full = 0;
		}

		if (readterm.aio_nbytes != 0 && (ret = aio_error(&readterm)) != EINPROGRESS) {
			if (ret != 0) {
				printf("reading from terminal error\n");
				exit(1);
			}
			lines = 0;
		}
		
		if (eof && empty) break;
	}
	
	free(remote_host);
	free(dir);
	freeaddrinfo(result);
	close(sock);
	return 0;
}
