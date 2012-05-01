#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define MAXL 25

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

int lines = 0;
int empty = 1, full = 0, eof = 0;
int nread = 0, nwrote = 0;
char buf[512];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condvar = PTHREAD_COND_INITIALIZER;

void *writer_thread(void *arg) {
	char *l, *t = buf + nwrote;
	int ret;

	pthread_mutex_lock(&mutex);
	while (!empty) {
		while (!empty)
			pthread_cond_wait(&condvar, &mutex);

		if (nwrote < nread) {
			l = buf + nread;
		} else {
			l = buf + sizeof(buf);
		}
		
		for (; t < l && *t != '\n'; t++);
		if (t < l) {
			t++;
			lines++;
		}

		pthread_mutex_unlock(&mutex);

		if (lines > MAXL) {
			char c[10];
			read(0, c, 1);
			lines = 0;
		}

		ret = write(1, buf + nwrote, t - buf - nwrote);
		pthread_mutex_lock(&mutex);

		if (ret < 0) {
			perror("write");
			exit(1);
		}
		nwrote += ret;
		full = 0;
		if (nwrote == sizeof(buf)) nwrote = 0;
		if (nwrote == nread) empty = 1;
		pthread_cond_signal(&condvar);
	}
	pthread_mutex_unlock(&mutex);
	return NULL;
}

void *reader_thread(void *arg) {
	int pos, rh_size, d_size;
	int status, sock, nbytes_sent, nbytes_recv, backlog;
	char *str = (char *) arg;
	char *remote_host, *dir, *port = "80";
	char request[255];
	struct addrinfo hints, *result, *rp;

	pos = delimpos(str, '/');
	if (pos != -1) {
		rh_size = pos + 2;
		d_size = strlen(str) - pos + 2;
	} else {
		rh_size = strlen(str) + 1;
		d_size = 2;
	}

	remote_host = (char *) malloc(rh_size * sizeof(char));
	dir = (char *) malloc(d_size * sizeof(char));

	if (pos != -1) {
		remote_host = strncpy(remote_host, str, pos);
		remote_host[strlen(remote_host)] = '\0';
		dir = strcpy(dir, str + pos);
	} else {
		remote_host = strcpy(remote_host, str);
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

	while (!eof) {
		int ret;
		while (!full)
			pthread_cond_wait(&condvar, &mutex);

		if (nread >= nwrote) {
			pthread_mutex_unlock(&mutex);
			ret = read(sock, buf+nread, sizeof(buf)-nread);
			pthread_mutex_lock(&mutex);
			if (ret == 0) {
				eof = 1;
				break;
			}
			nread += ret;
			if (nread == sizeof(buf)) nread = 0;
		} else {
			int tmp = nwrote;
			pthread_mutex_unlock(&mutex);
			ret = read(sock, buf+nread, tmp-nread);
			pthread_mutex_lock(&mutex);
			if (ret == 0) {
				eof = 1;
				break;
			}
		}

		if (nread == nwrote) full = 1;
		empty = 0;

		pthread_cond_signal(&condvar);
	}
	pthread_mutex_unlock(&mutex);

	free(remote_host);
	free(dir);	
	freeaddrinfo(result);
	close(sock);
	return NULL;
}

int main(int argc, char **argv) {
	pthread_t writer, reader;
	if (argc < 2) {
		printf("Usage: %s URL\n", argv[0]);
		exit(1);
	}

	pthread_create(&writer, NULL, writer_thread, NULL);
	pthread_create(&reader, NULL, reader_thread, (char *)argv[1]);
	pthread_join(reader, NULL);
	pthread_join(writer, NULL);
	return 0;
}
