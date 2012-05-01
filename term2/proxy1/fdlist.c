#include <sys/select.h>
#include <netdb.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#include "size.h"
#include "fdlist.h"

int
setfds(fd_set *readfds, fd_set *writefds,
        struct fdnode *fdl, int listensock)
{
        struct fdnode *t;
        int maxfd = 0;
        FD_ZERO(readfds);
        FD_ZERO(writefds);
        for (t = fdl; t != NULL; t = t->next) {
                if (t->fd > maxfd)
                        maxfd = t->fd;
                if (t->use) {
                        if (t->rw == 0)
                                FD_SET(t->fd, readfds);
                        if (t->rw == 1)
                                FD_SET(t->fd, writefds);
                }
        }
        for (t = fdl; t != NULL; t = t->next) {
                if (t->sout > maxfd)
                        maxfd = t->sout;
                if (t->use) {
                        if (t->rw == 0)
                                FD_SET(t->sout, writefds);
                        if (t->rw == 1 && t->cached == 0)
                                FD_SET(t->sout, readfds);
                }
        }
        FD_SET(listensock, readfds);
        if (listensock > maxfd)
                maxfd = listensock;
        return maxfd+1;
}

int fdladd(int fd, struct fdnode **head) {
        struct fdnode *t, *p = NULL;
        int flg;
        int new = 1;

        for (t = *head; t != NULL; t = t->next) {
                if (t->fd == fd && t->use == 0) {
                        /*free(t->url);
                        free(t->host);
                        free(t->uri);
                        freeaddrinfo(t->addr);
                        t->nwrote = 0;
                        t->nread = 0;
                        t->eof = 0;
                        t->empty = 1;
                        t->full = 0;
                        */
                        memset(t->buf, 0, sizeof(t->buf));
                        new = 0;
                        p = t;
                }
        }

        if (new) {
                p = (struct fdnode *) malloc(sizeof(struct fdnode));
                p->fd = fd;
        }

        p->use = 1;
        p->sout = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (p->sout == -1) {
                return -1;
        }
        /* NONBLOCK SocketOut */
        flg = fcntl(p->sout, F_GETFL);
        flg |= O_NONBLOCK;
        fcntl(p->sout, F_SETFL, flg);
        p->rw = 0;
        p->empty = 1;
        p->full = 0;
        p->nwrote = 0;
        p->nread = 0;
        p->eof = 0;
        p->cached = 0;

        if (!new) return p->sout;

        p->next = NULL;

        if (*head == NULL) {
                *head = p;
                return p->sout;
        }
        for (t = *head; t->next != NULL; t = t->next)
                ;
        t->next = p;
        return p->sout;
}

int fdlrem(int fd, struct fdnode *head) {
        struct fdnode *t = head;
        for (; t != NULL; t = t->next) {
                if (t->fd == fd) {
                        t->use = 0;
                        free(t->url);
                        free(t->host);
                        free(t->uri);
                        freeaddrinfo(t->addr);
                        close(t->fd);
                        close(t->sout);
                }
        }
}

int fdlfree(struct fdnode *head) {
        struct fdnode *t = head, *p;
        for (; t != NULL;) {
                p = t->next;
                freeaddrinfo(t->addr);
                free(t->url);
                free(t);
                t = p;
        }
        return 0;
}

