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

int makerequest(char *s, char *dir) {
	strcpy(s, "GET ");
	strcpy(s + strlen(s), dir);
	strcpy(s + strlen(s), " HTTP/1.0\r\n\r\n");
}

int main(int argc, char **argv) {
	int pos, rh_size, d_size;
	int lines = 0, maxlines = 25;
	int lowmark = 0, highmark = 0;
	int empty = 1, full = 0, eof = 0;
	int status, sock, nbytes_sent, nbytes_recv, backlog;
	char buf[512], *remote_host, *dir, *port = "http";
	char request[255];
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

	pos = 0;
	while (1) {
		fd_set readfds, writefds, errorfds;
		int ret;
		
		FD_ZERO(&readfds);
		FD_ZERO(&writefds);
		FD_SET(0, &readfds);
		FD_SET(sock, &readfds);
		FD_SET(1, &writefds);

		ret = select(sock+1, &readfds, NULL /*&writefds*/, NULL, NULL);
		printf("----");
		
		pos++;

		if (ret < 0) {
			perror("select");
			exit(1);
		}
		if (!full && !eof && FD_ISSET(sock, &readfds)) {
			if (highmark >= lowmark) {
				ret = read(sock, buf+highmark, sizeof(buf)-highmark);
				if (ret == 0) {
					eof = 1;
				} else {
					highmark += ret;
					if (highmark == sizeof(buf)) highmark = 0;
					if (highmark == lowmark) full = 1;
					empty = 0;
				}
			} else {
				ret = read(sock, buf+highmark, lowmark-highmark);
				if (ret == 0) {
					eof = 1;
				} else {
					highmark += ret;
					if (highmark == lowmark) full = 1;
					empty = 0;
				}
			}		
		}

		if (!empty && lines < maxlines /*&& FD_ISSET(1, &writefds)*/) {
			char *t = buf + lowmark;
			char *l;
			if (lowmark < highmark) {
				l = buf + highmark;
			} else {
				l = buf + sizeof(buf);
			}
			for(; t < l && *t != '\n'; t++);
			if (t < l) {
	               		t++;
	            		lines++;
	        	}
			ret = write(1, buf + lowmark, t - (buf + lowmark));
			if (ret <= 0) {
				perror("writing to terminal");
				exit(1);
			}
			lowmark += ret;
			full = 0;
			if (lowmark == sizeof(buf)) lowmark = 0;
			if (lowmark == highmark) empty = 1;
		}
		
		if (eof && empty) break;

		if (lines >= maxlines && FD_ISSET(0, &readfds)) {
			char tb[10];
			read(0, tb, 1);
			lines=0;
		}

		//if (pos == 20) break;
	}
	
	free(remote_host);
	free(dir);
	
	freeaddrinfo(result);
	close(sock);
	return 0;
}
