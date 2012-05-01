#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "cache.h"

struct cnode *cached(struct cnode *head, char url[]) {
        struct cnode *t = head;
        for (; t != NULL; t = t->next) {
                if (strstr(t->url, url) == t->url && strlen(t->url) == strlen(url) && t->completed == 1) {
                        return t;
		}
        }
        return NULL;
}

int createcache(struct cnode **head, char url[]) {
        struct cnode *t = *head, *p;
        p = (struct cnode *) malloc(sizeof(struct cnode));
        strcpy(p->url, url);
        p->size = 0;
        p->completed = 0;
        p->next = NULL;
        if (*head == NULL) {
                *head = p;
                return 0;
        }
        for (; t->next != NULL; t = t->next)
                ;
        t->next = p;
        return 0;
}

int addtocache(struct cnode *node, char content[], int size) {
        int i;
        for (i = 0; i < size && node->size+i < CACHESIZE; i++) {
                node->content[i+node->size] = content[i];
        }
	node->size += size;
        return 0;
}

int cacheit(struct cnode **head, char url[], char content[], int size) {
        struct cnode *t;
        if ((t = cached(*head, url)) == NULL) {
                createcache(head, url);
                addtocache(*head, content, size);
        } else {
                if (!t->completed) {
                        addtocache(t, content, size);
                }
        }
        return 0;
}

int completecache(struct cnode *head, char url[]) {
        struct cnode *t = head;
        for (; t != NULL; t = t->next) {
                if (strstr(t->url, url) == t->url && strlen(t->url) == strlen(url))
	t->completed = 1;
	}
}
