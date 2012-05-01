#ifndef _FDLIST_H_INCLUDED
#define _FDLIST_H_INCLUDED

struct fdnode {
        int fd; //Client fd
        int sout; //Server fd
        int use; // State of node
        int rw; // State of request being
        int empty;
        int full;
        int nread;
        int nwrote;
        int eof;
        int cached;
        struct cnode *cache;
        char *url;
        char *host;
        char *uri;
        char buf[BUFSIZE]; // Message buffer
        struct addrinfo *addr; // Server host address
        struct fdnode *next;
};

int setfds(fd_set *readfds, fd_set *writefds,
	struct fdnode *fdl, int listensock);
int fdladd(int fd, struct fdnode **head);
int fdlrem(int fd, struct fdnode *head);
int fdlfree(struct fdnode *head);
#endif
