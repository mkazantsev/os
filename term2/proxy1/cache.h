#ifndef _CACHE_H_INCLUDED
#define _CACHE_H_INCLUDED
#include "size.h"

struct cnode {
        char url[URLSIZE];
        char content[CACHESIZE];
        int size;
        int completed;
        struct cnode *next;
};

struct cnode *cached(struct cnode *head, char url[]);
int createcache(struct cnode **head, char url[]);
int addtocache(struct cnode *node, char content[], int size);
int cacheit(struct cnode **head, char url[], char content[], int size);
#endif
