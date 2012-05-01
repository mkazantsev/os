#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NPERTHREAD 10000000

struct pi_argument {
	long from;
	long to;
	double res;
};

void *pi_func(void *arg) {
	int i;
	double res = 0;
	struct pi_argument *rep = (struct pi_argument *) arg;
	for (i = rep->from; i < rep->to; i++) {
		res += 1.0/(i*4.0 + 1.0);
		res -= 1.0/(i*4.0 + 3.0);
	}
	rep->res = res;
	pthread_exit(rep);
	return rep;
}

int main(int argc, char **argv) {
	struct pi_argument *sums;
	double pi = 0;
	pthread_t *tids;
	int t_num;
	int i;
	if (argc < 2) {
		printf("specify a number of threads\n");
		exit(1);
	}
	
	t_num = atoi(argv[1]);
	
	sums = (struct pi_argument *) malloc(sizeof(struct pi_argument) * t_num);
	tids = (pthread_t *) malloc(sizeof(pthread_t) * t_num);

	for (i = 0; i < t_num; i++) {
		sums[i].from = i * NPERTHREAD;
		sums[i].to = (i+1) * NPERTHREAD;
	}
	
	for (i = 0; i < t_num; i++) {
		if (pthread_create(tids+i, NULL, pi_func, (void *)(sums+i)) != 0) {
			perror("create");
			exit(1);
		}
	}

	for (i = 0; i < t_num; i++) {
		struct pi_argument *res;
		if (pthread_join(tids[i], (void **)&res) != 0) {
			perror("join");
			exit(1);
		}
		pi += res->res;
		printf("res = %.20f\n", res->res);
	}

	pi *= 4.0;
	printf("pi = %.20f\n", pi);

	free(sums);
	free(tids);
	return 0;
}
