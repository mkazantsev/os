#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h> /* strlen */

struct list {
	char *s;
	pthread_mutex_t *mutex;
	struct list *next;
};

int done = 0;
struct list *head = NULL;

void swap(struct list *a, struct list *b) {
	char *s = b->s;
	b->s = a->s;
	a->s = s;
}

void *t_routine(void *arg) {
	while (!done) {
		usleep(5);
		if (head != NULL && head->next != NULL) {
			struct list *t, *p;
			pthread_mutex_lock(head->mutex);
			for (t = head; t != NULL; t = t->next) {
				for (p = t->next; p != NULL; p = p->next) {
					if (0 < strcmp(t->s, p->s))
						swap(t, p);
				}
			}
			pthread_mutex_unlock(head->mutex);
		}
	}
}

void putstate(void) {
	struct list *t;
	pthread_mutex_lock(head->mutex);
	for (t = head; t != NULL; t = t->next) {
		printf("%s -> ", t->s);
	}
	pthread_mutex_unlock(head->mutex);
	printf("\n");
}

int main(void) {
	pthread_t tid;
	struct list *t, *new;

	if (pthread_create(&tid, NULL, t_routine, NULL)) {
		perror("create");
		exit(1);
	}

	do {
		char *s = (char *) malloc(81*sizeof(char));
		fgets(s, 80, stdin);
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

		new = (struct list *) malloc(sizeof(struct list));
		new->s = s;
		new->mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(new->mutex, NULL);
		new->next = NULL;
		if (head == NULL) {
			head = new;
		} else {
			pthread_mutex_lock(head->mutex);
			new->next = head;
			head = new;
			pthread_mutex_unlock(head->mutex);
		}
	} while(1);

	pthread_join(tid, NULL);

	for (t = head; t != NULL;) {
		struct list *p = t->next;
		free(t->s);
		free(t->mutex);
		free(t);
		t = p;
	}
	return 0;
}
