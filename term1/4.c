#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct list {
    char *s;
    struct list *next;
};

int getstr(char *s) {
    memset(s, 0, 256);
    fgets(s, 255, stdin);
    s[strlen(s)-1] = '\0';
    if (s[0] == '.') return 0;
    return 1;
}

void writestr(char *s, struct list **node) {
    (*node) = (struct list *) malloc(sizeof(struct list));
    (*node)->s = (char *) malloc(sizeof(char)*(strlen(s)+1));
    strcpy((*node)->s, s);
    (*node)->next = 0;
}

int main(void) {
    char s[255];
    struct list *head, *prev, *cur;
    if (!getstr(s)) {
	printf("No input\n");
	return 0;
    }
    
    writestr(s, &head);
    prev = head;
    
    while (getstr(s)) {
	writestr(s, &cur);
	prev->next = cur;
	prev = cur;
    }
    
    printf("Input: ");
    for (cur = head; cur != 0; cur = cur->next) {
	printf("%s -> ", cur->s);
    }
    printf("\n");
    
    for (cur = head; cur != 0; cur = cur->next) {
	prev = cur->next;
	free(cur->s);
	free(cur);
	cur = prev;
    }
    
    return 0;
}
