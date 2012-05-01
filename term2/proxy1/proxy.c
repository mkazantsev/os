#include <sys/select.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "size.h"
#include "cache.h"
#include "http.h"
#include "fdlist.h"

extern errno;

//int listensock;

int sigint_handler() {
	//close(listensock);
	return 0;
}

int main(int argc, char *argv[]) {
	int status, new_fd;
	int listensock;
	struct addrinfo hints, *res;
	struct sockaddr saddr;
	socklen_t saddr_size = sizeof(saddr);
	char buf[BUFSIZE];
	int ret;
	struct fdnode *fdl = NULL, *writefdl = NULL, *t;
	struct cnode *cache = NULL;
	fd_set readfds, writefds;
	struct timeval timeout;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if (argc != 2) {
		printf("Usage: %s PORT\n", argv[0]);
		exit(1);
	}

	if (ret = getaddrinfo(NULL, argv[1], &hints, &res)) {
		printf("getaddrinfo: %s\n", gai_strerror(ret));
		exit(1);
	}

	if ((listensock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
		perror("socket");
		freeaddrinfo(res);
		exit(1);
	}

	if (bind(listensock, res->ai_addr, res->ai_addrlen) == -1) {
		perror("bind");
		freeaddrinfo(res);
		close(listensock);
		exit(1);
	}

	if (listen(listensock, MAX_CON_NUM) == -1) {
		perror("listen");
		freeaddrinfo(res);
		close(listensock);
		exit(1);
	}

	timeout.tv_sec = 60 * 3;
	timeout.tv_usec = 0;

	while (1) {
		struct cnode *c;
		int i = 0;
		ret = setfds(&readfds, &writefds, fdl, listensock);
		printf("wait for select\n");
		ret = select(ret, &readfds, &writefds, NULL, &timeout);
		printf("got selected %d fds\n", ret);

		if (ret < 0) {
			perror("select");
			break;
		}
		if (ret == 0) { /* timeout */
			printf(".\n");
			continue;
		}

		for (c = cache; c != NULL; c = c->next) {
			i++;
			printf("%s\n", c->url);
		}
		printf("Got %d cache pages\n", i);

		for (t = fdl; t != NULL; t = t->next) {
			if (FD_ISSET(t->fd, &readfds))
				printf("%d is ready to read\n", t->fd);
			if (FD_ISSET(t->fd, &writefds))
				printf("%d is ready to write\n", t->fd);
			if (FD_ISSET(t->sout, &readfds))
				printf("%d is ready to read\n", t->sout);
			if (FD_ISSET(t->sout, &writefds))
				printf("%d is ready to write\n", t->sout);
		}

		if (FD_ISSET(listensock, &readfds)) {
			new_fd = accept(listensock, &saddr, &saddr_size);
			if (new_fd == -1) {
				perror("accept");
				continue;
			}
			ret = fdladd(new_fd, &fdl);
			if (ret == -1) {
				perror("add socket");
				continue;
			}
			printf("Accepted new %d\n", new_fd);
		}

		for (t = fdl; t != NULL; t = t->next) {
			if (t->use && t->rw == 1 && !t->full && !t->eof && FD_ISSET(t->sout, &readfds)) {
				//memset(buf, 0, BUFSIZE);
				printf("to read from sout %d\n", t->sout);
				if (t->nread >= t->nwrote) {
					ret = read(t->sout, t->buf + t->nread, sizeof(t->buf)-t->nread);
					//cacheit(&cache, t->url, t->buf + t->nread, ret);
					if (ret == 0) {
						t->eof = 1;
						//completecache(cache, t->url);
						/*
						close(t->sout);
						fdlrem(t->fd, fdl);
						printf("Closed %d\n", t->sout);
						continue;
						*/
					} else {
						t->nread += ret;
						if (t->nread == sizeof(t->buf)) t->nread = 0;
						if (t->nread == t->nwrote) t->full = 1;
						t->empty = 0;
					}
				} else {
					ret = read(t->sout, t->buf + t->nread, t->nwrote-t->nread);
					//cacheit(&cache, t->url, t->buf + t->nread, ret);
					if (ret == 0) {
						t->eof = 1;
						//completecache(cache, t->url);
						/*
						close(t->sout);
						fdlrem(t->fd, fdl);
						printf("Closed %d\n", t->sout);
						continue;
						*/
					} else {
						t->nread += ret;
						if (t->nread == t->nwrote) t->full = 1;
						t->empty = 0;
					}
				}
			}
			if (t->use && t->rw == 0 && FD_ISSET(t->fd, &readfds)) {
				memset(buf, 0, BUFSIZE);
				printf("to read from sin %d\n", t->fd);
				ret = read(t->fd, buf, BUFSIZE);
				if (ret > 0) {
					int len;
					char *url;
					char *host;
					char *dir;

					write(1, buf, ret);

					/* Parse incoming request */
					if (parsereq(buf, ret, &url, &len) == -1) {
						close(t->fd);
						close(t->sout);
						t->use = 0;
						continue;
					}
					t->url = malloc((len+1)*sizeof(char));
					t->url = strncpy(t->url, url, len);
					t->url[len] = '\0';

					/* Parse given URL */
					parseurl(t->url, &len, &host, &dir);
					t->host = malloc((len+1)*sizeof(char));
					t->host = strncpy(t->host, host, len);
					t->host[len] = '\0';

					len = strlen(t->url) - len;
					t->uri = malloc((len+1)*sizeof(char));
					t->uri = strncpy(t->uri, dir, len);
					t->uri[len] = '\0';
					/*
					if (t->cache = cached(cache, t->url)) {
						t->cached = 1;
						t->rw = 1;
						continue;
					}
					*/
					if (ret = getaddrinfo(t->host, "http", &hints, &t->addr)) {
						printf("getaddrinfo: %s\n", gai_strerror(ret));
						write(t->fd, "HTTP/1.0 400\r\n\r\n", 17);
						close(t->fd);
						fdlrem(t->fd, fdl);
						continue;
					}

					if (connect(t->sout, t->addr->ai_addr, t->addr->ai_addrlen) == -1) {						
						if (errno == EINPROGRESS) {
							printf("wait for connection on %d\n", t->fd);
							continue;
						}
						close(t->fd);
						fdlrem(t->fd, fdl);
					}
					continue;
				}
				if (ret == 0) {
					close(t->fd);
					fdlrem(t->fd, fdl);
					printf("Closed %d\n", t->fd);
				}
			}
		//} /* readfds */
		//for (t = fdl; t != NULL; t = t->next) {
			if (t->use && t->rw == 0 && FD_ISSET(t->sout, &writefds)) {
				int len = strlen(t->uri) + 3 + 8 + 6;
				char *msg = malloc(len*sizeof(char));
				printf("to write to sout %d\n", t->sout);
				sprintf(msg, "GET %s HTTP/1.0\r\n\r\n", t->uri);
				if (write(t->sout, msg, len) <= 0) {
					perror("write sout");
					close(t->fd);
					fdlrem(t->fd, fdl);
					continue;
				}
				t->rw = 1;
				free(msg);
				printf("request on %d\n", t->sout);
			}
			if (t->use && t->rw == 1 && t->cached == 0 && !t->empty && FD_ISSET(t->fd, &writefds)) {
				char *l;
				if (t->nwrote < t->nread)
					l = t->buf + t->nread;
				else
					l = t->buf + sizeof(t->buf);
				//ret = write(t->fd, t->buf + t->nwrote, l - (t->buf + t->nwrote));
				ret = send(t->fd, t->buf + t->nwrote, l - (t->buf + t->nwrote), 0);
				if (ret <= 0) {
					perror("write sin");
					close(t->fd);
					fdlrem(t->fd, fdl);
					continue;
				}
				t->nwrote += ret;
				t->full = 0;
				if (t->nwrote == sizeof(t->buf)) t->nwrote = 0;
				if (t->nwrote == t->nread) t->empty = 1;
			}
			if (t->use && t->rw == 1 && t->cached == 1 && FD_ISSET(t->fd, &writefds)) {
				ret = write(t->fd, t->cache->content + t->nwrote, t->cache->size - t->nwrote);
				t->nwrote += ret;
				printf("work from cache for %s\n", t->url);
				if (t->nwrote == t->cache->size) {
					t->eof = 1;
					t->empty = 1;
				}
			}
			if (t->use && t->eof && t->empty) {
				fdlrem(t->fd, fdl);
				printf("Completed %d\n", t->fd);
				continue;
			}
		} /* fdl traverse */
	} /* while (1) */

	printf("gotcha");

	fdlfree(fdl);

	freeaddrinfo(res);
	close(listensock);
	return 0;
}
