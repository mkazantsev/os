#ifndef _HTTP_H_INCLUDED
#define _HTTP_H_INCLUDED
int parsereq(char req[], int size, char *url[], int *url_len);
int parseurl(char url[], int *host_len, char *host[], char *dir[]);
#endif
