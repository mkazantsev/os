#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#define NCHECK 1000000

int flag = 0;
long *nums;
long max;
int semid;
struct sembuf waitall;

struct pi_argument {
	int id;
	int total;
	long long done;
	long long extra;
	double res;
};

void getmax(int tnum) {
	int i;
	max = nums[0];
	for (i = 1; i < tnum; i++) {
		if (nums[i] > max)
			max = nums[i];
	}
}

void remsem_handler(int sig) {
	semctl(semid, 0, IPC_RMID, 0);
	exit(1);
}

void sigint_handler(int sig) {
	int tnum = waitall.sem_op * -1;
	struct sembuf setmax = {1, tnum, 0};
	flag = 1;
	sigset(SIGINT, remsem_handler);
	if (semop(semid, &waitall, 1) == -1) {
		perror("semop waitall");
		remsem_handler(0);
		exit(1);
	}
	getmax(tnum);
	if (semop(semid, &setmax, 1) == -1) {
		perror("semop setmax");
		remsem_handler(0);
		exit(1);
	}
}

void final_comp(struct pi_argument *arg) {
	struct sembuf touch = {0, 1, 0};
	struct sembuf waitmax = {1, -1, 0};
	long long i;
	nums[arg->id] = arg->done;
	if (semop(semid, &touch, 1) == -1) {
		perror("semop touch");
		remsem_handler(0);
		exit(1);
	}
	if (semop(semid, &waitmax, 1) == -1) {
		perror("semop waitmax");
		remsem_handler(0);
		exit(1);
	}

	arg->extra = max - arg->done;

	for (; arg->done <= max; arg->done++) {
		i = arg->done * arg->total + arg->id;
		arg->res += 1.0/(i*4.0 + 1.0);
		arg->res -= 1.0/(i*4.0 + 3.0);
	}
}

void *pi_func(void *arg) {
	struct pi_argument *rep = (struct pi_argument *) arg;
	long long i;
	for (rep->done = 0; ; rep->done++) {
		i = rep->done * rep->total + rep->id;
		rep->res += 1.0/(i*4.0 + 1.0);
		rep->res -= 1.0/(i*4.0 + 3.0);
		if (rep->done % NCHECK == 0) {
			if (flag) {
				final_comp(rep);
				break;
			}
		}
	}
	pthread_exit(rep);
	return rep;
}

int main(int argc, char **argv) {
	struct pi_argument *sums;
	double pi = 0;
	pthread_t *tids;
	int t_num;
	int i;
	key_t key = getuid();
	int flg = IPC_CREAT | 0666;
	if (argc < 2) {
		printf("specify a number of threads\n");
		exit(1);
	}
	
	t_num = atoi(argv[1]);

	waitall.sem_num = 0;
	waitall.sem_op = -t_num;
	waitall.sem_flg = 0;
	
	if ((semid = semget(key, 2, flg)) == -1) {
		perror("semget");
		exit(1);
	}

	sums = (struct pi_argument *) malloc(sizeof(struct pi_argument) * t_num);
	tids = (pthread_t *) malloc(sizeof(pthread_t) * t_num);
	nums = (long *) malloc(sizeof(long) * t_num);
	
	for (i = 0; i < t_num; i++) {
		sums[i].id = i;
		sums[i].total = t_num;
		sums[i].done = 0;
		sums[i].res = 0;
		sums[i].extra = 0;
	}
	
	for (i = 0; i < t_num; i++) {
		if (pthread_create(tids+i, NULL, pi_func, (void *)(sums+i)) != 0) {
			perror("create");
			exit(1);
		}
	}
	
	sigset(SIGINT, sigint_handler);
	//sigaction();

	for (i = 0; i < t_num; i++) {
		struct pi_argument *res;
		if (pthread_join(tids[i], (void **)&res) != 0) {
			perror("join");
			exit(1);
		}
		pi += res->res;
		printf("id %d res = %.20f extra = %d\n", res->id, res->res, res->extra);
	}
	
	pi *= 4.0;
	printf("pi = %.20f\n", pi);
	
	free(sums);
	free(tids);

	for (i = 0; i < t_num; i++) {
		printf("id %d iter = %d\n", i, nums[i]);
	}
	free(nums);
	remsem_handler(0);
	return 0;
}
