#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h> /* strlen */

struct list {
	char *s;
	struct list *next;
};

int done = 0;
struct list *head = NULL;
pthread_mutex_t mp = PTHREAD_MUTEX_INITIALIZER;

void swap(struct list *a, struct list *b) {
	char *s = b->s;
	b->s = a->s;
	a->s = s;
}

void *t_routine(void *arg) {
	while (!done) {
		sleep(5);
		pthread_mutex_lock(&mp);
		if (head != NULL && head->next != NULL) {
			struct list *t, *p;
			for (t = head; t != NULL; t = t->next) {
				for (p = t->next; p != NULL; p = p->next) {
					if (0 < strcmp(t->s, p->s))
						swap(t, p);
				}
			}
		}
		pthread_mutex_unlock(&mp);
	}
}

void putstate(void) {
	struct list *t;
	pthread_mutex_lock(&mp);
	for (t = head; t != NULL; t = t->next) {
		printf("%s -> ", t->s);
	}
	pthread_mutex_unlock(&mp);
	printf("\n");
}

int main(void) {
	pthread_t tid;
	struct list *t;

	if (pthread_create(&tid, NULL, t_routine, NULL)) {
		perror("create");
		exit(1);
	}

	do {
		char *s = (char *) malloc(81*sizeof(char));
		fgets(s, 10, stdin);
		if (strlen(s) == 1 && s[0] == '\n') {
			if (s[0] == '\n') {
				putstate();
				continue;
			}
		}
		s[strlen(s) - 1] = '\0';
		if (s[0] == '.') {
			free(s);
			done = 1;
			break;
		}

		struct list *new = (struct list *) malloc(sizeof(struct list));
		new->s = s;
		new->next = NULL;
		pthread_mutex_lock(&mp);
		if (head == NULL) {
			head = new;
		} else {
			new->next = head;
			head = new;
		}
		pthread_mutex_unlock(&mp);
	} while(1);

	pthread_join(tid, NULL);
	pthread_mutex_destroy(&mp);

	for (t = head; t != NULL;) {
		struct list *p = t->next;
		free(t->s);
		free(t);
		t = p;
	}
	return 0;
}
