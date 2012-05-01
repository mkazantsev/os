#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>
#include <ulimit.h>
extern char *optarg;
extern int optopt, optind, opterr;
extern char **environ;

int main(int argc, char *argv[]) {
	char options[ ] = "ispuU:cC:dvV:";
	int c, invalid = 0;
	long a;
	struct rlimit rlp;
	char **p;
	
	while ((c = getopt(argc, argv, options)) != EOF) {
		switch (c) {
		case 'i':
			printf("uid: %d, euid: %d, gid: %d, egid: %d\n", getuid(), geteuid(), getgid(), getegid());
			break;
		case 's':
			setpgid(0, 0);
			printf("Leader\n");
			break;
		case 'p':
			printf("pid: %d, ppid: %d, pgid: %d\n", getpid(), getppid(), getpgid(0));
			break;
		case 'u':
			printf("ulimit: %d\n", ulimit(1));
			break;
		case 'U':
			a = atol(optarg);
			if (a <= ulimit(1)) {
				ulimit(2, a);
				printf("New ulimit: %d\n", a);
			} else {
				printf("Too much to be done\n");
			}
			break;
		case 'c':
			getrlimit(RLIMIT_CORE, &rlp);
			printf("Current RLIMIT_CORE: %d\n", rlp.rlim_cur);
			break;
		case 'C':
			a = atoi(optarg);
			getrlimit(RLIMIT_CORE, &rlp);
			rlp.rlim_cur = a;
			setrlimit(RLIMIT_CORE, &rlp);
			break;
		case 'd':
			printf("%s\n", getenv("PWD"));
			break;
		case 'v':
			for (p = environ; *p; p++) {
				printf("%s\n", *p);
			}
			break;
		case 'V':
			putenv(optarg);
			break;
		case '?':
			printf("invalid option: %c\n", optopt);
			invalid++;
		}
	}
}
