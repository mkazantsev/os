#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

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

int main(int argc, char **argv) {
	int pos;
	int status, sock, nbytes_sent, nbytes_recv, backlog;
	char msgbuf[81], *remote_host, *dir, *port = "http";
	struct addrinfo hints, *result, *rp;
	
	if (argc < 2) {
		printf("Usage: %s URL\n", argv[0]);
		exit(1);
	}

	pos = delimpos(argv[0]);
	remote_host = (char *) malloc((pos + 2) * sizeof(char));
	dir = (char *) malloc((strlen(argv[0]) - pos + 1) * sizeof(char));
	
	remote_host = strncpy();

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = 0;
	hints.ai_protocol = IPPROTO_TCP;
	
	if ((status = getaddrinfo(argv[1], port, &hints, &result)) != 0) {
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
	if ((nbytes_sent = write(sock, "GET / HTTP/1.0\r\n\r\n", strlen("GET / HTTP/1.0\r\n\r\n"))) == -1) {
		perror("send");
		exit(1);
	}

	while (1) {
		fd_set readfds, writefds, errorfds;
		int ret;
		
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_SET(0, &readfds);
		FD_SET(sock, &readfds);
		FD_SET(1, &writefds);

		ret = select(sock+1, &readfds, &writefds, NULL, NULL);
		if (ret < 0) {
			perror("select");
			exit(1);
		}
	}
	while ((nbytes_recv = read(sock, msgbuf, 80)) != 0) {
		printf("%s", msgbuf);
	}
	
	free(remote_host);
	free(dir);
	
	freeaddrinfo(result);
	close(sock);
	return 0;
}
